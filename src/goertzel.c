#include <math.h>
#include <stdint.h>

#include "goertzel.h"

// Function to run the Goertzel algorithm on a set of samples for a specific frequency.
// Returns the magnitude squared of the targeted frequency component.
float goertzel_mag(int numSamples, float targetFrequency, float sampleRate, const float* data)
{
	int    k;
	float omega, sine, cosine, coeff, q0, q1, q2, magnitudeSquared;
	
	// Calculate the Goertzel parameters
	k = (int)(0.5f + ((numSamples * targetFrequency) / sampleRate));
	omega = (2.0f * (float)M_PI * k) / numSamples;
	sine = sinf(omega);
	cosine = cosf(omega);
	coeff = 2.0f * cosine;

	q0 = 0;
	q1 = 0;
	q2 = 0;

	// Run the Goertzel algorithm on the samples
	for (int i = 0; i < numSamples; i++)
	{
		q0 = coeff * q1 - q2 + data[i];
		q2 = q1;
		q1 = q0;
	}

	// Calculate the real and imaginary results, and compute the magnitude squared
	magnitudeSquared = q1 * q1 + q2 * q2 - q1 * q2 * coeff;
	
	return magnitudeSquared;
}

uint8_t goertzel_value(int numSamples, float sampleRate, float targetFrequency1, float targetFrequency2, const float* data)
{
	// Run the Goertzel algorithm for each target frequency
	float magnitudeSquared1 = goertzel_mag(numSamples, targetFrequency1, sampleRate, data);
	float magnitudeSquared2 = goertzel_mag(numSamples, targetFrequency2, sampleRate, data);

	// Compare and determine which frequency is present
	if (magnitudeSquared1 > magnitudeSquared2)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
