#include<stdio.h>
#include<stdlib.h>

typedef struct _RIFFHeader {
  char FileTypeBlocID[4]; // Identifier « RIFF »  (0x52, 0x49, 0x46, 0x46)
  int FileSize; // Overall file size minus 8 bytes
  char FileFormatID[4]; // Format = « WAVE »  (0x57, 0x41, 0x56, 0x45)
} RIFFHeader;

typedef struct _DataFormatHeader {
  char FormatBlocID[4]; // Identifier « fmt␣ »  (0x66, 0x6D, 0x74, 0x20)
  int BlocSize; // Chunk size minus 8 bytes, which is 16 bytes here  (0x10)
  short AudioFormat; // Audio format (1: PCM integer, 3: IEEE 754 float)
  short NbrChannels; // Number of Number of channels
  int Frequency; // Sample rate (in hertz)
  int BytePerSec; // Number of bytes to read per second (Frequency * BytePerBloc);
  short BytePerBloc; // Number of bytes per block (NbrChannels * BitsPerSample / 8)
  short BitsPerSample; // Number of bits per sample
} DataFormatHeader;

typedef struct _wavHeader {
  RIFFHeader riffHeader;
  DataFormatHeader dataFormatHeader;
} wavHeader;
