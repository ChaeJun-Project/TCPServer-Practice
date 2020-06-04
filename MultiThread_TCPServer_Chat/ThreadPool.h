#pragma once
#include <thread>
#include <functional>
#include <queue>
#include <vector>
#include <future>

//������ Ǯ(�̱��� ����)
//https://modoocode.com/285 ����
class ThreadPool final
{
public:
	static ThreadPool* Get()
	{
		static ThreadPool* instance = new ThreadPool(10);
		return instance;
	}

	ThreadPool(int thread_num);
	~ThreadPool();
    
	//�۾� ť�� �۾�(job)�� �߰��ϴ� �Լ�
	//������ Ŭ�󿡼� �����͸� �ְ� �޴� �Լ��� ��ȯ���� ��� void�̱� ������ void()�� �Ű������� ����
	void EnqueueJob(std::function<void()> job);

private:
	//������ ����
	void WorkThread();

    //�� �������� ����
    int thread_num;
	//�����带 �����ϴ� ����
	std::vector<std::thread> threads;
	//�� �ϵ�(job)�� �����ϴ� ť
	std::queue<std::function<void()>> jobs;
	//ť�� ��Ƽ ������ ȯ�濡���� �������� �ʱ� ������ ť�� �۾�ó���� ������ ��ȣ��ġ
	std::condition_variable cv_jod_q;
	//���ؽ� ����ȭ�� ����ϱ� ���� ����
	std::mutex m_job_q;

	//��� ������ ����
	bool stop_all;
};
