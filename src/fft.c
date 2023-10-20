#include <stdio.h>
#include <math.h>
#include <complex.h> // Use the complex library available in C99

#include "fft.h"

void fft_recursive(float complex* x, float complex* X, int n) {
	// Base case
	if (n == 1) {
		X[0] = x[0];
		return;
	}

	// Recursive FFT for even and odd parts
	float complex even[n / 2];
	float complex odd[n / 2];

	for (int i = 0; i < n / 2; i++) {
		even[i] = x[2 * i];
		odd[i] = x[2 * i + 1];
	}

	float complex Even[n / 2];
	float complex Odd[n / 2];

	fft_recursive(even, Even, n / 2);
	fft_recursive(odd, Odd, n / 2);

	// Combining parts
	for (int i = 0; i < n / 2; i++) {
		float complex twiddle = cexp(-2.0 * I * PI * i / n) * Odd[i];
		X[i] = Even[i] + twiddle;
		X[i + n / 2] = Even[i] - twiddle;
	}
}

void fft_real(float* x, float complex* X, int n) {
	float complex x_complex[N];

	// Convert real to complex
	for (int i = 0; i < N; i++) {
		x_complex[i] = x[i];
	}

	// Use the general-purpose FFT function
	float complex X_full[N];
	fft_recursive(x_complex, X_full, n);

	// Now, copy only the non-redundant part of the spectrum into X
	for (int i = 0; i < N / 2 + 1; i++) {
		X[i] = X_full[i];
	}
}
