#include <stdio.h>
#include<math.h>
#define _USE_MATH_DEFINES

#define SAMPLE_N 16 
#define FREQUENCY_N 8

void inverseDFT(float *frequencies, int frequencyN, float *cosAmp, float *sinAmp, int sampleN, float *samples){
	for(int i=0;i<sampleN;i++){
		samples[i] = 0;
		float signalProportion = (float)i/sampleN;
		printf("i: %d   signalProportion: %f\n", i, signalProportion);
		for(int j=0;j<frequencyN;j++){
			float angle;
			angle = signalProportion*frequencies[j]*M_PI*2;
			printf("i: %d    frequency[%d]: %f     angle: %f\n", i, j, frequencies[j], angle);
			samples[i] += cosAmp[j]*cos(angle) - sinAmp[j]*sin(angle);
		}
	}
}

void DFT(float *samples, int sampleN, float *frequencies, int frequencyN, float *cosAmp, float *sinAmp){
	float sampleCopy[SAMPLE_N];
	for(int i=0;i<sampleN;i++){
		sampleCopy[i] = samples[i];
	}

	for(int i=0;i<frequencyN;i++){
		float cosMultSum=0;
		float sinMultSum=0;
		for(int j=0;j<sampleN;j++){
			float signalProportion = (float)j/sampleN;
			float frequency = frequencies[i];
			float angle = signalProportion*frequency*M_PI*2;
			float compareCosSample = cos(angle);
			cosMultSum += compareCosSample * samples[j];
			float compareSinSample = sin(angle);
			sinMultSum -= compareSinSample * samples[j];
			printf("frequencyI: %d frequency: %f  sampleI: %d cosMult: %f    sinMult: %f\n", i, frequency, j, compareCosSample * samples[j], -compareSinSample * samples[j]);
		}
		printf("frequencyI: %d  cosMultSum: %f    sinMultSum: %f\n", i, cosMultSum/sampleN*2.0f, sinMultSum/sampleN*2.0f);

		cosAmp[i] = cosMultSum/sampleN*2.0f;
		sinAmp[i] = sinMultSum/sampleN*2.0f;
	}
}

/*
void FFT(float *samples, int sampleN, float *frequencies, int frequencyN, float *cosAmp, float *sinAmp){
	float evenSamples[SAMPLE_N];
	float oddSamples[SAMPLE_N];

	if(sampleN <= 1){
		return samples[0];
	}
}
*/

int main(){
	float samples[SAMPLE_N];
	float frequencies[FREQUENCY_N];
	float cosAmps[FREQUENCY_N];
	float sinAmps[FREQUENCY_N];

	for(int i=0;i<FREQUENCY_N;i++){
		frequencies[i] = 0;
		cosAmps[i] = 0;
		sinAmps[i] = 0;
	}

	int frequencyN = 4;
	frequencies[0] = 1;
	frequencies[1] = 2;
	frequencies[2] = 3;
	frequencies[3] = 4;
	frequencies[4] = 4;
	frequencies[5] = 5;
	frequencies[6] = 6;
	frequencies[7] = 7;

	cosAmps[0] = 1;
	cosAmps[1] = 4;
	cosAmps[2] = 1;
	cosAmps[3] = 2;
	cosAmps[4] = 3.5;
	cosAmps[5] = 3.5;
	cosAmps[6] = 3.5;
	cosAmps[7] = 3.5;

	sinAmps[0] = 1;
	sinAmps[1] = 4;
	sinAmps[2] = 1;
	sinAmps[3] = 2;
	sinAmps[4] = 3.5;
	sinAmps[5] = 3.5;
	sinAmps[6] = 3.5;
	sinAmps[7] = 3.5;

	inverseDFT(frequencies, frequencyN, cosAmps, sinAmps, SAMPLE_N, samples);

	for(int i=0;i<SAMPLE_N;i++){
		printf("samples[%d]: %f\n", i, samples[i]);
	}
	for(int i=0;i<frequencyN;i++){
		printf("Before DFT cosAmps[%d]: %f sinAmps[%d]: %f\n", i, cosAmps[i], i, sinAmps[i]);
	}


	DFT(samples, SAMPLE_N, frequencies, frequencyN, cosAmps, sinAmps);
	for(int i=0;i<frequencyN;i++){
		printf("cosAmps[%d]: %f sinAmps[%d]: %f\n", i, cosAmps[i], i, sinAmps[i]);
	}

	inverseDFT(frequencies, frequencyN, cosAmps, sinAmps, SAMPLE_N, samples);
	for(int i=0;i<SAMPLE_N;i++){
		printf("Reconstructed samples[%d]: %f\n", i, samples[i]);
	}
	return 0;
}
