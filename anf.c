#include "anf.h"

int anf(int v, int *X, int *A, int *rho, int* index)
{
    /*
     v in Q15: newly captured sample
     X in Q11: X[3] databuffer - Reserve a sufficiently number of integer bits such that summing numbers does not cause overflow (Hence no shift is needed after summing numbers)
     A in Q14: the adaptive coefficient
     e in Q11: output signal
     rho (fixed) in Q15: rho[2]={rho,rho^2}
     index points to t-1 sample (t current time index) in X -> circular buffer
     */
    int e,k, rho_sqr, new_X;
    long AC0,AC1;

    //k = *index;
    AC0= (long) lambda*(*(rho));                //Q30
    AC1 = (long) (32768-lambda)* (*(rho+1));    //Q30
    AC0+= AC1;                                      //Q30
    *rho = (int) (AC0>>15);                     //Q15
    AC0 = (long)(*rho)*(*rho);                  //Q30
    rho_sqr= (int) (AC0>>15);                   //Q15;

    AC0 = (long) (*(rho)) * (*(X));                 // AC0 = rho * x(t-1)                   // Q26  Q15*Q11
    AC0 = (long) (*A) * (int)(AC0>>14);             // AC0 = rho * a(t-1) * x(t-1)          //Q26 MSB A(Q14)*AC0(Q12)= Q26
    AC1 = (long) -(*A) * (*(X));                    // AC1 = -a(t-1)*x(t-1)                 // Q25   Q14*Q11=Q25
    //k++;                                            // k points to the previous value of X
    //if(k>=3) k=0;                                   // if k goes out of bounds reset it to the first value
    AC1 += (long) (*(index))<<14;                   // AC1 = -a(t-1)*x(t-1) + x(t-2)        // AC1 (Q25) = AC1(Q25) + (*(X+k))<<14 (Q25)
    long dummy = (long) (rho_sqr) * (*(index));     // dummy = rho_sqr * x(t-2)             //dummy (Q26) = rho(Q15)* (*(X+k)) (Q11)
    AC0 -= dummy;                                   // AC0 = rho*a(t-1)*x(t-1) - rho_sqr*x(t-2) //AC0 (Q26)= AC0(Q26)- dummy(Q26)
    //k++;                                            // k points to the previous value of X
    //if(k>=3) k=0;                                   // if k goes out of bounds reset it to the first value}
    new_X =  (v>>4) + (int)(AC0>>15);               // x(t) = y(t) + rho(t)a(t-1)x(t-1) - rho_sprt*x(t-2) // *(X+k)(Q11) = v(Q11) + AC0(Q11)
    //xa =... = *(X(k));
    e = (int) new_X + (int)(AC1>>14);               // e(Q11) = *(X+k)(Q11) + AC1(Q11)
    //k++;                                            // k points to the previous value of X
    //if(k>=3) k=0;                                   // if k goes out of bounds reset it to the first value
    AC0 = (long) mu * (*(X));                       // ACO = 2mu * x(t-1) //AC0(Q26) = mu(Q15) * (*(X+k))(Q11)        //IS mu int?
    AC0 = (long) e * (int)(AC0>>15);                // AC0 = e * 2mu * x(t-1) //AC0(Q26) = AC0(Q15)* e(Q11)
    *A = (*A) + (int)(AC0>>12);                     // A = a(t-1) + 2mu * e * x(t-1) //A(Q14) = A(Q14) + AC0(14)

    if( *A >= 32768 ){
        *A = 32768;
    }else if( *A <= -32768 ){
        *A = -32768;
    }

    /*
    k--;                                            // k points to the next value of X                                    )
    if(k<0) k=2;                                    // if k goes out of bounds reset it to the last value
    *index = k;                                     // update index for next iteration
    */

    *index = *X;
    *X = new_X;
    
    return e;
}
