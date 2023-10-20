#ifndef fft_h
#define fft_h

#include <complex.h> // Use the complex library available in C99

#define PI 3.14159265358979323846
#define N 64 // Number of points for the FFT

void fft_real(float* x, float complex* X, int n);
void fft_recursive(float complex* x, float complex* X, int n);

#endif