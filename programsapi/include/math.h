#ifndef MATH_H
#define MATH_H

#define PI 3.14159265359
#define E 2.71828182846

float power(float base, int exp); // Returns x raised to the power of y.
double sqrt(double x); // Returns square root of x.
unsigned fact(unsigned x); // Returns factoriel of x.
double dmod(double x, double y); // Modulo for double.

float sin(double deg); // Returns the sine of a radian angle x.
float cos(double deg); // Returns the cosine of a radian angle x.
double tan(double x); // Returns the tangent of a radian angle x.

#endif