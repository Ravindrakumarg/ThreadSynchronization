
#include <iostream>
#include <string.h>
#include <Windows.h>

#define THREAD_COUNT	4

void WriteToBuffer(const char *);
char* ReadFromBuffer();
void DestroyBuffer();
void CreateEvents();
void CloseEvent();

char * buffer = nullptr;
HANDLE ghWriteEvent;
HANDLE gThreads[THREAD_COUNT];
DWORD WINAPI ThreadFunc(LPVOID);

int main()
{
	DWORD waitResult;
	buffer = (char*)malloc(100);

	CreateEvents();

	WriteToBuffer("Hello how are you!");
	std::cout<<"buffer: " << ReadFromBuffer() << std::endl;

	waitResult = WaitForMultipleObjects(THREAD_COUNT, gThreads, TRUE, INFINITE);

	switch(waitResult)
	{
	case WAIT_OBJECT_0:
		std::cout << "All thread ended" << std::endl;
		break;
	default:
		std::cout << "WaitForMultipleObjects failed=" << GetLastError() << std::endl;
	}

	CloseEvent();

	getchar();

	return 0;
}

void CreateEvents()
{
	DWORD	threadid;

	ghWriteEvent = CreateEvent(nullptr, TRUE, FALSE, TEXT("WriteEvent"));
	if (ghWriteEvent == nullptr)
	{
		std::cout << "event creation failed" << std::endl;
		return;
	}

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		gThreads[i] = CreateThread(nullptr, 0, ThreadFunc, nullptr, 0, &threadid);

		if (gThreads[i] == nullptr)
		{
			std::cout << "Thread creation failed" << std::endl;
			return;
		}
	}
}

void CloseEvent()
{
	CloseHandle(ghWriteEvent);
}

DWORD WINAPI ThreadFunc(LPVOID param)
{
	UNREFERENCED_PARAMETER(param);

	printf("Thread %d waiting for write event...\n", GetCurrentThreadId());

	DWORD waitResult;

	waitResult = WaitForSingleObject(ghWriteEvent, INFINITE);
	switch (waitResult)
	{
	case WAIT_OBJECT_0:
		std::cout << "Read by Threadid:" << GetCurrentThreadId() << ", data: " << ReadFromBuffer() << std::endl;
		break;
	default:
		std::cout << "wait state failed" << GetLastError() << std::endl;
		return 0;
	}

	std::cout << "Thread " << GetCurrentThreadId() << " exiting" << std::endl;
	return 1;
}

void WriteToBuffer(const char * pBuffer)
{
	memcpy_s(buffer, sizeof(buffer), pBuffer, sizeof(pBuffer));
	buffer[sizeof(pBuffer) + 1] = '\n';

	if (!SetEvent(ghWriteEvent))
	{
		std::cout << "Set event failed" << std::endl;
		return;
	}
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