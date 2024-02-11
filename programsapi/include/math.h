#ifndef MATH_H
#define MATH_H

long pow(int base, unsigned exp); // Returns x raised to the power of y.
double sqrt(double x); // Returns square root of x.
unsigned fact(unsigned x); // Returns factoriel of x.

// Trigonometry functions are calculated via Taylor's series

double sin(double x); // Returns the sine of a radian angle x.
double cos(double x); // Returns the cosine of a radian angle x.
double tan(double x); // Returns the tangent of a radian angle x.
double cotan(double x); // Returns the cosine of a radian angle x.

double asin(double x); // Returns the arc sine of x in radians.
double acos(double x); // Returns the arc cosine of x in radians.
double atan(double x); // Returns the arc tangent of x in radians.

#endif