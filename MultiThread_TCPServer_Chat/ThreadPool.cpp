#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(int thread_num)
{
	this->thread_num = thread_num;
	stop_all = false;
	//vector.reserve�ʿ� ���� �޸𸮰� ���Ҵ� �Ǵ� ���� ������.
	//���ʹ� �޸��� ũ�Ⱑ ������ ��� ����� ��� ��� �����͸� �� �޸𸮿� �����ϰ�
	//������ �޸𸮿� ����� ��� ��ü�� �Ҹ��Ű�� ���� �޸𸮸� ����
	//�Ҵ� -> ���� -> �Ҹ� -> ������ ����� ���� ���� ����
	//������ Ǯ������ �ִ� �������� ������ 10���� �����Ͽ��⿡ ��� ������
	threads.reserve(thread_num);

	//�̸� ������ ����Լ� WorkThreads�� �����ϴ� �� 10���� �����带
	//������ ���Ϳ� �߰�
	for (int i = 0; i < thread_num; ++i)
	{
		threads.emplace_back([this]() { this->WorkThread(); });
	}
}

ThreadPool::~ThreadPool()
{
	stop_all = true;
	cv_jod_q.notify_all(); //���� ������ ��ٸ��� ��� �����带 ����. ��, ����ϴ� ��� �����带 ����

	//threads ���Ϳ� ����� ��� ��Ҹ� ����
	for (auto& thread : threads)
	{
		thread.join(); //�� �����尡 ����� �� ���� ��ٸ�. �����尡 ���� �߿� ���α׷��� ����Ǹ� �����尡 ���������� ������� �ʱ� ����
	}
}

void ThreadPool::EnqueueJob(std::function<void()> job)
{
	if (stop_all)
	{
	    std::cout<< "��� �����尡 �۾��� �����Ͽ����ϴ�." <<std::endl;
		return;
	}

	//lock_guard�� lock ��� �� unlock�� �����ʾ� �߻��ϴ� ����� ������ �����ϱ� ���� ���
	//�۾� ť�� �۾��� �־���ϱ⿡ ����ȭ�� �ǽ�
	std::lock_guard<std::mutex> lock(m_job_q);
	jobs.push(std::move(job));

	//��� ���� �����带 �ϳ� ����
	cv_jod_q.notify_one();
}

void ThreadPool::WorkThread()
{
	//������ ����
	while (true)
	{
		//����ȭ ����
		std::unique_lock<std::mutex> lock(m_job_q);

		//�۾� ť�� ������� �۾��� �����ְų� stop_all�� ������ �� ���� �ٸ� ��������� ���
		cv_jod_q.wait(lock, [this]() { return !this->jobs.empty() || stop_all; });

		//�۾� ť�� ������� �۾��� ���� �� ������ ������ ����
		if (stop_all && this->jobs.empty())
		{ 
			return;
		}

		//�۾� ť�� ����� ���� ù ��° �۾��� ����
		std::function<void()> job = std::move(jobs.front());
		//���� ù ��° �۾��� �۾� ť���� ����
		jobs.pop();

		//����ȭ ����
		lock.unlock();

		//�Ҵ���� �۾��� �����尡 ����
		job();
	}
}