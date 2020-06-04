#pragma once

class Thread 
{
public:
    Thread() = default;

	void CloseThread(); //스레드 종료

	bool StartThread();
	void JoinThread();


protected:
	virtual bool Run() = 0;

private:
      HANDLE thread_handle = nullptr;
	  DWORD thread_id = 0;

	  static bool WINAPI StaticRunThread(LPVOID lpParam); //클래스 멤버 함수를 스레드로 사용하기 위해서는 정적으로 선언해야 함
};