#include<stdio.h>
#include<stdlib.h>
#include"wav.h"
#define EXAMPLE_WAV_FILE "examples/rejoice.wav"

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
  char *sampleBuffer = malloc(sizeof(char)*samplePartSize);
  readSize = fread(sampleBuffer, 1, samplePartSize, file);
  printf("readSize: %zu\n", readSize);
  printf("sampleBuffer first four bytes: %c%c%c%c\n", sampleBuffer[0], sampleBuffer[1], sampleBuffer[2], sampleBuffer[3]);
  fclose(file);

  FILE* outputFile;
  wavHeader newHeader = header;
  newHeader.riffHeader.FileSize = newHeader.riffHeader.FileSize + 192000*10;
  outputFile = fopen("addedSilence.wav", "w");
  fwrite(&newHeader, sizeof(newHeader), 1, outputFile);
  fwrite("data", sizeof(4), 1, outputFile);
  char *zeroBuffer = calloc(sizeof(char)*192000*10, 1);
  size_t writtenSize = 0;
  printf("sizeof zeroBuffer: %zu\n", sizeof(zeroBuffer));
  writtenSize = fwrite(zeroBuffer, 1, 192000*10, outputFile);
  printf("writtenSize: %zu\n", writtenSize);
  fwrite(sampleBuffer+4, samplePartSize-4, 1, outputFile);
  free(sampleBuffer);

  fclose(outputFile);
  return 0;
}
