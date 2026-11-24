#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"wav.h"
#define EXAMPLE_WAV_FILE "examples/rejoice.wav"
typedef unsigned char byte;

byte getMeanInRange(byte* samples, int start, int end){
  int rangeSize = end - start;
  byte mean;
  if(rangeSize > 1024){
    int meanA, meanB;
    meanA = getMeanInRange(samples, start, (start+end)/2);
    meanB = getMeanInRange(samples, (start+end)/2, end);
    mean = (meanA + meanB)/2;
  }
  else{
    int total=0;
    for(int i=start;i<end;i++){
      total += samples[i];
    }
    mean = total/rangeSize;
  }
  return mean;
}

void replaceWithMean(byte* samples, int start, int end){
  if(end < start){
    printf("replaceWithMean range start and end in wrong order!\n");
    return;
  }
  byte mean = getMeanInRange(samples, start, end);
  if(end-start > 2048){
    printf("start: %d  end: %d  mean: %d\n", start, end, mean);
  }
  for(int i=start;i<end;i++){
    samples[i] = mean;
  }
}

void replaceWithFirst(byte* samples, int start, int end){
  if(end < start){
    printf("replaceWithMean range start and end in wrong order!\n");
    return;
  }
  byte first = samples[start];
  if(end-start > 2048){
    printf("start: %d  end: %d  first: %d\n", start, end, first);
  }
  for(int i=start;i<end;i++){
    samples[i] = first;
  }
}

void quantize(byte* samples, int start, int end, int quantN){
  if(end < start){
    printf("quantize range start and end in wrong order!\n");
    return;
  }
  for(int i=start;i<end;i++){
    int quantUnit = 256/quantN;
    samples[i] = (samples[i]/quantUnit)*quantUnit;
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

  /*
  for(int windowN = 32; windowN <= 16384; windowN*=2){
    FILE* outputFile;
    wavHeader newHeader = header;
    byte *newBuffer = malloc(sizeof(byte)*samplePartSize);
    memcpy(newBuffer, sampleBuffer, samplePartSize);
    for(int i=0;i<samplePartSize-windowN;i+=windowN){
      if(windowN > 2048){
        printf("i: %d\n", i);
      }
      int start, end;
      start = i;
      end = (i+windowN<samplePartSize)?i+windowN:samplePartSize;
      replaceWithFirst(newBuffer, start, end);
    }

    char outputFilePath[50];
    sprintf(outputFilePath, "firstSample%d.wav", windowN);
    outputFile = fopen(outputFilePath, "w");
    printf("writing %s!\n", outputFilePath);
    fwrite(&newHeader, sizeof(newHeader), 1, outputFile);
    fwrite("data", sizeof(4), 1, outputFile);
    size_t writtenSize = 0;
    printf("sizeof newBuffer: %zu\n", sizeof(newBuffer));
    printf("writtenSize: %zu\n", writtenSize);
    fwrite(newBuffer, samplePartSize, 1, outputFile);
    free(newBuffer);

    fclose(outputFile);
  }
  */

  for(int quantI=2;quantI<=128;quantI*=2){
    char outputFilePath[50];
    sprintf(outputFilePath, "quantized%d.wav", quantI);
    FILE* outputFile = fopen(outputFilePath, "w");
    wavHeader newHeader = header;
    byte *newBuffer = malloc(sizeof(byte)*samplePartSize);
    memcpy(newBuffer, sampleBuffer, samplePartSize);
    quantize(newBuffer, 0, samplePartSize, quantI);
    fwrite(&newHeader, sizeof(newHeader), 1, outputFile);
    fwrite("data", sizeof(4), 1, outputFile);
    size_t writtenSize = 0;
    printf("sizeof newBuffer: %zu\n", sizeof(newBuffer));
    printf("writtenSize: %zu\n", writtenSize);
    fwrite(newBuffer, samplePartSize, 1, outputFile);
    free(newBuffer);
  }
  free(sampleBuffer);
  return 0;
}
