#include <stdio.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define BYTES_PER_SECOND 176400
#define FILE_SIZE BYTES_PER_SECOND*1*60*15
#define PI 3.141592

void intTo4Char(int val, char* result){
	result[0] = (val >> 24) & 0xff;
	result[1] = (val >> 16) & 0xff;
	result[2] = (val >> 8) & 0xff;
	result[3] = val & 0xff;
}

int writeHeader(FILE *out){
	int headerLen = 0;

	// FileTypeBlocID (4 bytes)
	fprintf(out, "RIFF");
	headerLen += 4;

	// FileSize (4 bytes)
	int fileSize = FILE_SIZE;
	char fileSizeBuffer[4];
	intTo4Char(FILE_SIZE, fileSizeBuffer);
	fprintf(out, "%c%c%c%c", fileSizeBuffer[3], fileSizeBuffer[2], fileSizeBuffer[1], fileSizeBuffer[0]);
	headerLen += 4;

	// FileFormatID (4 bytes)
	fprintf(out, "WAVE");
	headerLen += 4;

	// FormatBlocID (4 bytes)
	fprintf(out, "fmt%c", 0x20);
	headerLen += 4;

	// BlockSize (4 bytes)
	int blockSize = 16;
	char blockSizeBuffer[4];
	intTo4Char(blockSize, blockSizeBuffer);
	fprintf(out, "%c%c%c%c", blockSizeBuffer[3], blockSizeBuffer[2], blockSizeBuffer[1], blockSizeBuffer[0]);
	headerLen += 4;

	// AudioFormat (2 bytes)
	int formatType = 1;
	fprintf(out, "%c%c", formatType & 0xff, (formatType>>8) & 0xff);
	headerLen += 2;
	
	// NbrChannels (2 bytes)
	int channelN = 2;
	fprintf(out, "%c%c", channelN & 0xff, (channelN>>8) & 0xff);
	headerLen += 2;
	
	// Frequency (4 bytes)
	int sampleRate = SAMPLE_RATE;
	char sampleRateBuffer[4];
	intTo4Char(sampleRate, sampleRateBuffer);
	fprintf(out, "%c%c%c%c", sampleRateBuffer[3], sampleRateBuffer[2], sampleRateBuffer[1], sampleRateBuffer[0]);
	headerLen += 4;
	
	int bitsPerSample = 16;
	// BytesPerSec (4 bytes)
	int bytesPerSec = sampleRate * bitsPerSample * channelN/8;
	char bytesPerSecBuffer[4];
	intTo4Char(bytesPerSec, bytesPerSecBuffer);
	printf("bytesPerSec: %d\n",bytesPerSec);
	fprintf(out, "%c%c%c%c",bytesPerSecBuffer[3], bytesPerSecBuffer[2], bytesPerSecBuffer[1], bytesPerSecBuffer[0]);
	headerLen += 4;
	printf("bytesPerSec: %d\n",bytesPerSec);
	
	// BytePerBloc (2 bytes)
	int bytesPerBlock = bitsPerSample*channelN/8;
	fprintf(out, "%c%c", bytesPerBlock&0xff, (bytesPerBlock>>8)&0xff);
	printf("bytesPerBlock: %d\n",bytesPerBlock);
	headerLen += 2;
	
	// BitsPerSample (2 bytes)
	fprintf(out, "%c%c", bitsPerSample&0xff, (bitsPerSample>>8)&0xff);
	printf("bytesPerSample: %d\n",bitsPerSample);
	headerLen += 2;

	return headerLen;
}

short getSinSample(int frameIdx, int frameN, int intensity, int period){
	double sampleDouble = intensity * (sin(frameIdx * PI * period / SAMPLE_RATE));
	return (short)sampleDouble;
}
void writeSinWave(FILE *out, int dataSize){
	fprintf(out, "data");
	int dataSizeBuffer[4];
	intTo4Char(dataSize/8, dataSizeBuffer);
	printf("dataSize: %d\n",dataSize);
	fprintf(out, "%c%c%c%c", dataSizeBuffer[3], dataSizeBuffer[2], dataSizeBuffer[1], dataSizeBuffer[0]);

	for(int i=0;i<dataSize/4;i++){
		short intensity1 = getSinSample(i, dataSize/4, 256*64, 261);
		short intensity2 = getSinSample(i, dataSize/4, 256*64, 330);
		short intensity3 = getSinSample(i, dataSize/4, 256*64, 392);

		short leftSample, rightSample;
		leftSample = intensity1+intensity2;
		rightSample = intensity1+intensity2;

		fprintf(out, "%c%c", leftSample&0xff, (leftSample>>8)&0xff);
		fprintf(out, "%c%c", rightSample&0xff, (rightSample>>8)&0xff);
	}
}

int main(int argc, char** argv){
	char resultFile[] = "result.wav";
	FILE *out = fopen(resultFile, "wb");
	int headerSize = writeHeader(out);
	printf("headerSize: %d\n",headerSize);
	writeSinWave(out, FILE_SIZE-headerSize);

	fclose(out);
	return 0;
}
