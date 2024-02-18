#include "math.h"

#define MINDIFF 2.25e-308 // use for convergence check
#define TERMS 7


float power(float base, int exp) {
    if(exp < 0) {
        if(base == 0)
            return -0; // Error! 0^0
        return 1 / (base * power(base, (-exp) - 1));
    }
    if(exp == 0)
        return 1;
    if(exp == 1)
        return base;
    return base * power(base, exp - 1);
}

double sqrt(double x)
{
    double root=x/3, last, diff=1;
    if (x <= 0) return 0;
    do {
        last = root;
        root = (root + x / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}

unsigned fact(unsigned x)
{
    return x <= 0 ? 1 : x * fact(x-1);
}

double dmod(double x, double y) {
    return x - (int)(x/y) * y;
}

float sin(double deg) {
    deg = dmod(deg, 360.0f);
    float rad = deg * PI / 180;
    float sin = 0;

    int i;
    for(i = 0; i < TERMS; i++) 
    { 
        sin += power(-1, i) * power(rad, 2 * i + 1) / fact(2 * i + 1);
    }
    return sin;
}

float cos(double deg) {
    deg = dmod(deg, 360.0f); 
    float rad = deg * PI / 180;
    float cos = 0;

    int i;
    for(i = 0; i < TERMS; i++) 
    { 
        cos += pow(-1, i) * pow(rad, 2 * i) / fact(2 * i);
    }
    return cos;
}

double tan(double x)
{
    return sin(x)/cos(x);
}