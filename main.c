/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/*  main.c                                                                   */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   TMS320C5515 ANF-LMS implementation   				                     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#include "stdio.h"
#include "anf.h"

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  main( )                                                                 *
 *                                                                          *
 * ------------------------------------------------------------------------ */
int main( void )
{
	int v,e,tmp1;
	//int index=0 ;
	FILE  *fpv;
	FILE  *fpo;
	char  tempc[2];

	int U[2]={0,0};
	int A[1]={16384};
	int rho[2]={29491, 29491};  //rho=0.9, rho^2

	printf( "\n***Program has Started***\n" );

	fpv = fopen("..\\data\\in.pcm", "rb");
	fpo = fopen("..\\data\\out.pcm", "wb");

	if (fpv == NULL){
	    printf("Can't open input file\n");
	    return 0;
	}
	if (fpo == NULL){
	    printf("Can't open output file\n");
	    return 0;
	}

	printf( "1\n" );

    	/*
    	 * tempc = pointer to a block of memory with a minimum size of sizeof(char)*2 bytes.
    	 * each element read is one byte sizeof(char). 2 elements are read out at a time
    	 * (hence tempc[2], this is in total one data object that is read consisting of 2
    	 * elements each a char wide)
    	 * fread returns the total amount of elements read to tmp1 (which should be 2)
    	 */

	//Begin filtering the data
    while (fread(tempc, sizeof(char), 2, fpv) == 2){
        printf( "In here\n" );
        v = (tempc[0]&0xFF)|(tempc[1]<<8);      // concatenate two bytes into a word
        /*
         * v = data
         * U = data buffer
         * A =  adaptive coeficient
         * rho
         * index = points to t-1 sample in U
         */
    	e = anf(v,&U[0],&A[0],&rho[0],&U[0]);  // Adaptive Notch Filter.
    	tempc[0] = (e&0xFF);
    	tempc[1] = (e>>8)&0xFF;

    	fwrite(tempc, sizeof(char), 2, fpo);
    }

    printf( "out while\n" );

    	fclose(fpv);

    	printf( "\n***Program has Terminated***\n" );
}

/*****************************************************************************/
/* End of main.c                                                             */
/*****************************************************************************/

