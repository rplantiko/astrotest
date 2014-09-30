#include "lcg.h"

//--------------------------------------------------------------------  
// LCG pseudo random number generator
// from Bruce Schneier, Cryptographic Algorithms
//--------------------------------------------------------------------  
long s1=241524,s2=1416271;


#define MODMULT(a, b, c, m, s) q = s/a; s = b*(s-a*q)-c*q; if (s < 0) s += m;

// LCG Zufallszahlengenerator
double randLCG(void)
{
    long q, z;

    MODMULT(53668, 40014, 12211, 2147483563L, s1)
    MODMULT(52774, 40692, 3791, 2147483399L, s2)
    z = s1 - s2;
    if (z < 1)
        z += 2147483562;
    return z * 4.656613e-10;
}

void initLCG(long init_s1, long init_s2)
{
    s1 = init_s1;
    s2 = init_s2;
}

