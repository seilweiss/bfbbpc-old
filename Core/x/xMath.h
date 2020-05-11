#ifndef XMATH_H
#define XMATH_H

#define PI 3.1415927f
#define DEG2RAD(x) ((x) * PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / PI)

void xMathInit();
void xMathExit();
float xatof(const char *x);
void xsrand(unsigned int seed);
unsigned int xrand();
float xAngleClampFast(float a);
float xDangleClamp(float a);

#endif