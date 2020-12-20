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
	int index=0 ;
	FILE  *fpv;
	FILE  *fpo;
	char  tempc[2];

	int U[3]={0,0,0};
	int A[1]={16384};
	int rho[2]={29491, 29491};  //rho=0.9, rho^2

	printf( "\n***Program has Started***\n" );

	fpv = fopen("..\\data\\in.pcm", "rb");
	printf( "reading file opening\n" );
	fpo = fopen("..\\data\\out.pcm", "wb");
	printf( "writing file opening\n" );

	if (fpv == NULL){
	    printf("Can't open input file\n");
	    return 0;
	}
	if (fpo == NULL){
	    printf("Can't open output file\n");
	    return 0;
	}

	fseek(fpv, 0, SEEK_SET);

	tmp1 = fread(tempc, sizeof(char), 2, fpv);
	if(ferror(fpv)!=0){
	    perror("Error");
	}

	//Begin filtering the data
    while (tmp1 == 2){
        v = (tempc[0]&0xFF)|(tempc[1]<<8);      // concatenate two bytes into a word
        /*
         * v = data
         * U = data buffer
         * A =  adaptive coeficient
         * rho
         * index = points to t-1 sample in U
         */
        e = anf(v,&U[0],&A[0],&rho[0],&U[1]);  // Adaptive Notch Filter. for anf.c
    	//e = anf(v,&U[0],&A[0],&rho[0],&index);  // Adaptive Notch Filter. for anf.asm
    	tempc[0] = (e&0xFF);
    	tempc[1] = (e>>8)&0xFF;

    	fwrite(tempc, sizeof(char), 2, fpo);
    	tmp1 = fread(tempc, sizeof(char), 2, fpv);
    }

    printf( "out while now\n" );

    fclose(fpv);
    fclose(fpo);

    printf( "\n***Program has Terminated***\n" );
}
/*****************************************************************************/
/* End of main.c                                                             */
/*****************************************************************************/

