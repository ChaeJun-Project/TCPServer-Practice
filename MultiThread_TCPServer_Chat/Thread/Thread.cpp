#include "stdafx.h"
#include "Thread.h"

void Thread::CloseThread()
{
	if (thread_handle)
	{
		CloseHandle(thread_handle);
    }
}

bool Thread::StartThread()
{
	if (thread_handle)
	{
		if (WaitForSingleObject(thread_handle, 0) == WAIT_TIMEOUT)
		{
			return false;
		}

		CloseHandle(thread_handle);
	}

	//스레드 생성
	thread_handle = CreateThread(
	nullptr, 
	0, 
	(LPTHREAD_START_ROUTINE)Thread::StaticRunThread,
	this,
	0,
	&thread_id
	);

	//스레드 생성에 성공하면 true 반환
	if(!thread_handle)
	   return true;

    //스레드 생성에 실패하면 false 반환
	else
      return false;
}

void Thread::JoinThread()
{
  //thread_handle 오브젝트가 소유한 스레드가 종료될 때까지 대기
  WaitForSingleObject(thread_handle, INFINITE); 
}

bool __stdcall Thread::StaticRunThread(LPVOID lpParam)
{
	Thread* thread = (Thread*)lpParam;
	return thread->Run();
}
