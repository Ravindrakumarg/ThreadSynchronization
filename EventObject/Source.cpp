
#include <iostream>
#include <string.h>

void WriteToBuffer(const char *);
char* ReadFromBuffer();
void DestroyBuffer();

char * buffer = nullptr;

int main()
{
	WriteToBuffer("Hello how are you!");
	ReadFromBuffer();
	return 0;
}

void WriteToBuffer(const char * pBuffer)
{
	buffer = (char*)malloc(sizeof(pBuffer) + 1);
	strcpy_s(buffer, sizeof(buffer),pBuffer);
}

char * ReadFromBuffer()
{
	return buffer;
}

void DestroyBuffer()
{
	if (buffer)
		delete buffer;

	buffer = nullptr;
}