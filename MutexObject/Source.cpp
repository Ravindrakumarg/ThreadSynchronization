
#include <iostream>
#include <Windows.h>
#include <string.h>
#include <string>

#define THREAD_COUNT	4
#define MSG "How are you"

void Setup();
char* GetData();
void WriteData(const char*, DWORD counter = 0);
void CloseMutex();
DWORD WINAPI ThreadFunc(LPVOID param);

HANDLE mutex;
char * buffer;
HANDLE lThread[THREAD_COUNT];

int main()
{
	buffer = (char*)malloc(100);
	memset(buffer, '\0', strlen(buffer));
	
	WriteData(MSG);

	Setup();

	DWORD waitStatus = WaitForMultipleObjects(THREAD_COUNT, lThread, TRUE, INFINITE);
	switch (waitStatus)
	{
	case WAIT_OBJECT_0:
		std::cout << "All threads closed" << std::endl;
		break;
	default:
		std::cout << "failed in waiting status. Error: "<< GetLastError() << std::endl;
	}

	CloseMutex();

	getchar();
	return 0;
}

void Setup()
{
	DWORD pThreadID;

	mutex = CreateMutex(nullptr, FALSE, "WriterMutex");
	if (mutex == nullptr)
	{
		std::cout << "mutex creation failed:" << GetLastError() << std::endl;
		return;
	}

	for (int i = 0; i < THREAD_COUNT; i++)
	{
		lThread[i] = CreateThread(nullptr, 0, ThreadFunc, nullptr, 0, &pThreadID);

		if (lThread[i] == nullptr)
		{
			std::cout << "Thread creation failed, thread no:" << i << " with error: " << GetLastError() << std::endl;
		}
	}
}

DWORD WINAPI ThreadFunc(LPVOID param)
{
	UNREFERENCED_PARAMETER(param);
	DWORD counter = 1;

	while (counter < 5)
	{
		DWORD waitStatus = WaitForSingleObject(mutex, INFINITE);

		switch (waitStatus)
		{
		case WAIT_OBJECT_0:
			__try {
				std::cout << " Count:"<< counter << "Reading of buffer by thread: " << GetCurrentThreadId() << " Data: " << GetData() << std::endl;
				WriteData(MSG, counter);
				counter++;
			}

			__finally {
				if (!ReleaseMutex(mutex)) {
					std::cout << "failed to release mutex, error: " << GetLastError() << std::endl;
				}
			}
			break;
		default:
			std::cout << "faild in reading wait status" << std::endl;
			return -1;
		}
	}

	return 1;
}
char* GetData()
{
	return buffer;
}
void CloseMutex()
{
	for (int i = 0; i < THREAD_COUNT; ++i)
	{
		CloseHandle(lThread[i]);
	}

	CloseHandle(mutex);
}

void WriteData(const char *pBuffer, DWORD counter)
{
	memcpy(buffer, pBuffer, strlen(pBuffer));
}
