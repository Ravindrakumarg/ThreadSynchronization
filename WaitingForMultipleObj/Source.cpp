
#include <iostream>
#include <Windows.h>

#define EVENT_COUNT	2

HANDLE gEvents[2];

DWORD	WINAPI	ThreadProc(LPVOID);

int main()
{
	HANDLE thread;
	DWORD dwThreadID, waitEvent;

	for (int counter = 0; counter < EVENT_COUNT; ++counter)
	{
		gEvents[counter] = CreateEvent(nullptr, true, false, NULL);
		
		if (gEvents[counter] == nullptr)
		{
			std::cout << "create event failed" << std::endl;
			return -1;
		}
	}

	thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &dwThreadID);
	
	if (thread == nullptr)
	{
		std::cout << "Create thread failed" << std::endl;
		return -1;
	}

	while (1)
	{
		waitEvent = WaitForMultipleObjects(2, gEvents, TRUE, 5000);

		switch (waitEvent)
		{
		case WAIT_OBJECT_0:
			std::cout << "first event set" << std::endl;
			break;
		case WAIT_OBJECT_0 + 1:
			std::cout << "second event set" << std::endl;
			break;
		case WAIT_TIMEOUT:
			std::cout << "time out" << std::endl;
			break;
		default:
			std::cout << "wait error. Error code=" << GetLastError() << std::endl;
			ExitProcess(0);
		}
	}
	for (int counter = 0; counter < EVENT_COUNT; ++counter)
	{
		CloseHandle(gEvents[counter]);
	}

	getchar();
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID param)
{
	UNREFERENCED_PARAMETER(param);

	if (!SetEvent(gEvents[0]))
	{
		std::cout << "Setevent failed" << std::endl;
	}

	return 0;
}