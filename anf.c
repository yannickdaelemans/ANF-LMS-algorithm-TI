#include "anf.h"

int old_data = 0;

int anf(int v, int *X, int *A, int *rho, unsigned int* index)
{
    /*
     v in Q?: newly captured sample
     X in Q?: X[3] databuffer - Reserve a sufficiently number of integer bits such that summing numbers does not cause overflow (Hence no shift is needed after summing numbers)
     A in Q?: the adaptive coefficient
     e in Q?: output signal
     rho (fixed) in Q?: rho[2]={rho,rho^2}
     index points to t-2 sample (t current time index) in X -> circular buffer
     changed index myself, used to be t-1
     */

    int e,k;
    long AC0,AC1;

    k=*index;

	//add your own code here
    // is A that weird symbol?
    *rho = ((*A)*(*rho)) + (1-(*A)*(*(rho+1));
    AC0 = v + ((*rho)*(*X)*old_data) - ((*rho)*(*rho)*(k));
    AC1 = AC0 - (old_data*(*X)) + k;
    e = old_data + (2*AC1*(*X)*mu);
    
    old_data = e;
    return e;
}
