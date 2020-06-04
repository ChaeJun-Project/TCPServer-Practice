#pragma once

class Thread 
{
public:
    Thread() = default;

	void CloseThread(); //������ ����

	bool StartThread();
	void JoinThread();


protected:
	virtual bool Run() = 0;

private:
      HANDLE thread_handle = nullptr;
	  DWORD thread_id = 0;

	  static bool WINAPI StaticRunThread(LPVOID lpParam); //Ŭ���� ��� �Լ��� ������� ����ϱ� ���ؼ��� �������� �����ؾ� ��
};