#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <complex.h>
#include"wav.h"
#include"fft.h"
#define EXAMPLE_WAV_FILE "examples/rejoice8.wav"
#define WINDOW_N 2048 
#define QUANT_N 2 
#define ZERO_FREQ_N 2048 
typedef unsigned char byte;

void fftChange(float* samples, int start, int end){
  cplx buf[WINDOW_N];
  for(int i=0;i<WINDOW_N;i++){
    if(i+start>end){
      buf[i] = 0;
    }
    else{
      float val = samples[i+start];
      buf[i] = val;
    }
  }

  fft(buf, WINDOW_N);
  // quantize low frequencies
  for(int i=0;i<ZERO_FREQ_N;i++){
    float real, img;
    real = creal(buf[i]);
    img = cimag(buf[i]);
    if(real != 0.0){
      printf("prev real: %f    ", real);
    }
    real = (int)(real*QUANT_N)/(float)QUANT_N;
    img = (int)(img*QUANT_N)/(float)QUANT_N;
    if(real != 0.0){
      printf("%lf\n", real);
    }
    buf[i] = real + img*I;

    if(i > 0 && i < WINDOW_N /2){
      buf[WINDOW_N - i] = real - img * I;
    }
  }
  ifft(buf, WINDOW_N);
  for(int i=start;i<=end;i++){
    float realVal = creal(buf[i-start]);
    if(realVal>1.0){
      realVal = 1.0;
    }
    if(realVal<-1.0){
      realVal = -1.0;
    }
    samples[i] = realVal;
  }
}

int main(int argc, char** argv){
  wavHeader header;
  FILE *file;
  size_t readSize;
  file = fopen(EXAMPLE_WAV_FILE, "r");
  readSize = fread(&header, sizeof(header), 1, file);
  printf("readSize: %zu\n", readSize);

  printf("header->riffHeader->FileTypeBlocID: %c%c%c%c\n", header.riffHeader.FileTypeBlocID[0], header.riffHeader.FileTypeBlocID[1], header.riffHeader.FileTypeBlocID[2], header.riffHeader.FileTypeBlocID[3]);
  printf("header->riffHeader->FileSize: %d\n", header.riffHeader.FileSize);
  printf("header->riffHeader->FileFormatID: %c%c%c%c\n", header.riffHeader.FileFormatID[0], header.riffHeader.FileFormatID[1], header.riffHeader.FileFormatID[2], header.riffHeader.FileFormatID[3]);
  printf("\n\n");

  printf("header->dataFormatHeader->FormatBlocID: %c%c%c%c\n", header.dataFormatHeader.FormatBlocID[0], header.dataFormatHeader.FormatBlocID[1], header.dataFormatHeader.FormatBlocID[2], header.dataFormatHeader.FormatBlocID[3]);
  printf("header->dataFormatHeader->BlocSize: %d\n", header.dataFormatHeader.BlocSize);
  printf("header->dataFormatHeader->audioFormat: %d\n", header.dataFormatHeader.AudioFormat);
  printf("header->dataFormatHeader->NbrChannels: %d\n", header.dataFormatHeader.NbrChannels);
  printf("header->dataFormatHeader->Frequency: %d\n", header.dataFormatHeader.Frequency);
  printf("header->dataFormatHeader->BytePerSec: %d\n", header.dataFormatHeader.BytePerSec);
  printf("header->dataFormatHeader->BytePerBloc: %d\n", header.dataFormatHeader.BytePerBloc);
  printf("header->dataFormatHeader->BitsPerSample: %d\n", header.dataFormatHeader.BitsPerSample);

  int samplePartSize = header.riffHeader.FileSize-sizeof(wavHeader)+8;
  byte *sampleBuffer = malloc(sizeof(byte)*samplePartSize);
  readSize = fread(sampleBuffer, 1, samplePartSize, file);
  printf("readSize: %zu\n", readSize);
  printf("sampleBuffer first four bytes: %c%c%c%c\n", sampleBuffer[0], sampleBuffer[1], sampleBuffer[2], sampleBuffer[3]);
  fclose(file);

  float sineWin[WINDOW_N];
  for(int i=0;i<WINDOW_N;i++){
    // hann[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / (float)(WINDOW_N -1)));
    sineWin[i] = sin((float)M_PI * i / (float)(WINDOW_N-1));
  }

  char outputFilePath[50];
  sprintf(outputFilePath, "%dFFTd.wav", WINDOW_N);
  FILE* outputFile = fopen(outputFilePath, "w");
  wavHeader newHeader = header;
  float prevWindow[WINDOW_N];
  float currentWindow[WINDOW_N];
  memset(prevWindow, 0, WINDOW_N);
  byte *realSamples = sampleBuffer+8;
  samplePartSize -= 8;
  byte *newBuffer = malloc(sizeof(byte)*samplePartSize);
  // memcpy(newBuffer, sampleBuffer, samplePartSize);
  for(int i=0;i<samplePartSize;i+=WINDOW_N/2){
    int start, end;
    start = i;
    end = i+WINDOW_N-1;
    end = end>samplePartSize-1?samplePartSize-1:end;
    for(int j=0;j<=end-start;j++){
      currentWindow[j] = ((float)realSamples[j+start]-128.0)/128.0;
    }

    for(int j=0;j<WINDOW_N;j++){
      currentWindow[j] *= sineWin[j];
    }
    fftChange(currentWindow, 0, end-start);

    for(int j=0;j<WINDOW_N;j++){
      currentWindow[j] *= sineWin[j];
    }

    if(start == 0){ // If first window
      // printf("start: %d  end: %d  prevStart: %d  prevEnd: %d  First window\n", start, end, -1, -1);
      for(int j=0;j<WINDOW_N/2;j++){
        newBuffer[j+start] = currentWindow[j];
      }
    }
    else if(start+WINDOW_N>=samplePartSize-1){ // If last window
      // printf("start: %d  end: %d  prevStart: %d  prevEnd: %d  Last window\n", start, end, start-WINDOW_N/2, start+WINDOW_N/2-1);
      for(int j=0;j<WINDOW_N/2;j++){
        newBuffer[j+start] = currentWindow[j] + prevWindow[j+WINDOW_N/2];
      }
      for(int j=WINDOW_N/2;j<WINDOW_N;j++){
        newBuffer[j+start] = currentWindow[j];
      }
    }
    else {
        for (int j = 0; j < WINDOW_N / 2; j++) {
          float value = currentWindow[j] + prevWindow[j + WINDOW_N / 2];
          newBuffer[start + j] = (byte)(value*128+127.0);
        }
    }
    

    for(int j=0;j<WINDOW_N;j++){
      prevWindow[j] = currentWindow[j];
    }
  }
  fwrite(&newHeader, sizeof(newHeader), 1, outputFile);
  fwrite("data", sizeof(4), 1, outputFile);
  fwrite(&samplePartSize, sizeof(4), 1, outputFile);
  size_t writtenSize = 0;
  printf("sizeof newBuffer: %zu\n", sizeof(newBuffer));
  writtenSize = fwrite(newBuffer, samplePartSize, 1, outputFile);
  printf("writtenSize: %zu\n", writtenSize);
  free(newBuffer);

  free(sampleBuffer);
  return 0;
}
