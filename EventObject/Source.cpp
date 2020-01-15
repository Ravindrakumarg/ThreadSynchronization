
#include <iostream>
#include <string.h>
#include <Windows.h>

/* Applications can use event objects in a number of situations to notify a waiting thread of the occurrence of an event.For example, \
overlapped I / O operations on files, named pipes, and communications devices use an event object to signal their completion.
The following example uses event objects to prevent several threads from reading from a shared memory buffer while a master thread is writing to that buffer.
First, the master thread uses the CreateEvent function to create a manual - reset event object whose initial state is nonsignaled.
Then it creates several reader threads.The master thread performs a write operation and then sets the event object to the signaled state when 
it has finished writing.*/

/* Event object is use here for shynchronisation  How it works is actually main thread will create one event in non single mode and it will also start multiple threads 
 and all thread will wait for that thread to get single. And that thread will get single once main thread will done writing into the buffer.*/

#define THREAD_COUNT	4

void WriteToBuffer(const char *);
char* ReadFromBuffer();
void DestroyBuffer();
void CreateEvents();
void CloseEvent();

char * buffer = nullptr;				// share resource where main thread will try to write and other reader threads will try to read
HANDLE ghWriteEvent;					// this is the event which will get used for shyncronisation
HANDLE gThreads[THREAD_COUNT];			// will have handler for 4 reader threads
DWORD WINAPI ThreadFunc(LPVOID);		// thread function

int main()
{
	DWORD waitResult;
	buffer = (char*)malloc(100);
	memset(buffer, '\0', strlen(buffer));

	CreateEvents();						// here we will create event and threads

	WriteToBuffer("Hello how are you!");	// here main thread will write into the buffer and single to the event
	std::cout<<"buffer: " << ReadFromBuffer() << std::endl;

	// main thread is waiting for all thread to exit
	waitResult = WaitForMultipleObjects(THREAD_COUNT, gThreads, TRUE, INFINITE);

	switch(waitResult)
	{
	case WAIT_OBJECT_0:
		std::cout << "All thread ended" << std::endl;
		break;
	default:
		std::cout << "WaitForMultipleObjects failed=" << GetLastError() << std::endl;
	}

	CloseEvent();		// close all thread
	DestroyBuffer();	// destroy the buffer

	getchar();

	return 0;
}

void CreateEvents()
{
	DWORD	threadid;

	ghWriteEvent = CreateEvent(nullptr, TRUE, FALSE, TEXT("WriteEvent"));	// create a event in non single mode
	if (ghWriteEvent == nullptr)
	{
		std::cout << "event creation failed" << std::endl;
		return;
	}

	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		gThreads[i] = CreateThread(nullptr, 0, ThreadFunc, nullptr, 0, &threadid);	// create 4(THREAD_COUNT) thread

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
	
	// here all thread will wait till the time event(ghWriteEvent) will get notify.
	waitResult = WaitForSingleObject(ghWriteEvent, INFINITE);
	switch (waitResult)
	{
	case WAIT_OBJECT_0:
		// once thread will get single then it will start reading from the buffer.
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

	// this will turn the event in signal mode and which will be recived by all thread which are waiting for this evnet to turn up
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