#include "anf.h"
#include "stdio.h"

int anf(int raw_data, int *X_min_1, int *A_min_1, int *rho, int *X_min_2)
{
    /*
     raw_data   in Q15: newly captured sample
     X_min_1    in Q11:
     X_min_2    in Q11
     A_min_1    in Q14: the adaptive coefficient
     output     in Q11: output signal
     rho        in Q15: rho[2]={rho,rho^2}
     */
    int output, rho_sqr, new_X;
    long AC0,AC1;

    //calculate rho(t)
    AC0= (long) lambda * (*(rho));                  // AC0 = lambda * rho(t-1)
    AC1 = (long) (32768 - lambda) * (*(rho+1));     // AC1 = (1-lambda) * rho(inf)
    AC0+= AC1;                                      // AC0 = lambda * rho(t-1) + (1-lambda) * rho(inf) Q30
    *rho = (int) (AC0>>15);                         // make Q30 to Q15 so it fits inside an integer
    AC0 = (long)(*rho)*(*rho);                      // AC0 = rho^2
    rho_sqr= (int) (AC0>>15);                       // make Q15 again so it fits inside an integer

    // calculate x(t)
    AC0 = (long) (*(rho)) * (*(X_min_1));               // AC0 = rho * x(t-1)                       // Q26  Q15*Q11
    AC0 = (long) (*A_min_1) * (int)(AC0>>14);           // AC0 = rho * a(t-1) * x(t-1)              // Q26  A(Q14)*AC0(Q12)
    AC0 -= (long) (rho_sqr) * (*(X_min_2));;            // AC0 = rho*a(t-1)*x(t-1) - rho_sqr*x(t-2)
    new_X =  (raw_data>>4) + (int)(AC0>>15);            // x(t) = y(t) + rho(t)a(t-1)x(t-1) - rho_sprt*x(t-2) // Q11

    // calculate e(t) = output modified data
    AC1 = (long) -(*A_min_1) * (*(X_min_1));            // AC1 = -a(t-1)*x(t-1)                 // Q25
    AC1 += (long) (*(X_min_2))<<14;                     // AC1 = -a(t-1)*x(t-1) + x(t-2)        // Q25
    output = (int) new_X + (int)(AC1>>14);              // e = new_X -a(t-1)*x(t-1) + x(t-2)    // Q11

    // calculate a(t)
    AC0 = (long) mu * (*(X_min_1));                     // ACO = 2mu * x(t-1)                   // Q26
    AC0 = (long) output * (int)(AC0>>15);               // AC0 = e * 2mu * x(t-1)               // Q26
    *A_min_1 = (*A_min_1) + (int)(AC0>>12);             // A = a(t-1) + 2mu * e * x(t-1)        // Q14

    if( *A_min_1 >= 32768 ){
        *A_min_1 = 32768;
    }else if( *A_min_1 <= -32768 ){
        *A_min_1 = -32768;
    }


    *X_min_2 = *X_min_1;
    *X_min_1 = new_X;

    return output;
}
