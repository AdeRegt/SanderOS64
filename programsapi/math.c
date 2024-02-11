#include "math.h"

#define TRIG_PREC 30 // calculates to what precision calculate trigonometry functions

long pow(int base, unsigned exp)
{
    long res = base;
    for (int i = 1; i < ++exp; i++)
        base *= base;

    return res;
}

double sqrt(double x)
{
}

unsigned fact(unsigned x)
{
    int res = 1;

    for (int i = 1; i < x + 1; i++)
        res *= i;

    return res;
}

double sin(double x)
{
    double numerator = 0;
    double denominator = 0;
    double result = 0;

    // brojnik = [(-1)^k] * x ^ (2k+1)
    // nazivnik = (2k+1)!
    for (int i = 0; i < TRIG_PREC; i += 2)
    {
        numerator += pow(x, 2*i + 1);
        denominator += fact(2*i + 1);
        result += numerator / denominator;
        numerator = 0;
        denominator = 0;
    }

    for (int i = 1; i < TRIG_PREC; i += 2)
    {
        numerator -= pow(x, 2*i + 1);
        denominator += fact(2*i + 1);
        result += numerator / denominator;
        numerator = 0;
        denominator = 0;
    }

    return result;
}

double cos(double x)
{

}

double tan(double x)
{
    return sin(x)/cos(x);
}

double cotan(double x) 
{

}

double asin(double x)
{

}

double acos(double x)
{

}

double atan(double x)
{

}
