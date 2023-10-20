#ifndef _goertzel_h_
#define _goertzel_h_

float goertzel_mag(int numSamples, float targetFrequency, float sampleRate, const float* data);
uint8_t goertzel_value(int numSamples, float sampleRate, float targetFrequency1, float targetFrequency2, const float* data);

#endif