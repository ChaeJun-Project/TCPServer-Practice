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

	//������ ����
	thread_handle = CreateThread(
	nullptr, 
	0, 
	(LPTHREAD_START_ROUTINE)Thread::StaticRunThread,
	this,
	0,
	&thread_id
	);

	//������ ������ �����ϸ� true ��ȯ
	if(!thread_handle)
	   return true;

    //������ ������ �����ϸ� false ��ȯ
	else
      return false;
}

void Thread::JoinThread()
{
  //thread_handle ������Ʈ�� ������ �����尡 ����� ������ ���
  WaitForSingleObject(thread_handle, INFINITE); 
}

bool __stdcall Thread::StaticRunThread(LPVOID lpParam)
{
	Thread* thread = (Thread*)lpParam;
	return thread->Run();
}
