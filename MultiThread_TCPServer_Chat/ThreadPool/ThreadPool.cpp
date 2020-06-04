#include "stdafx.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool(int thread_num)
{
	this->thread_num = thread_num;
	stop_all = false;
	//vector.reserve필요 없이 메모리가 재할당 되는 것을 막아줌.
	//벡터는 메모리의 크기가 부족할 경우 저장된 모든 요소 데이터를 새 메모리에 복사하고
	//원래의 메모리에 저장된 모든 객체를 소멸시키고 원래 메모리를 해제
	//할당 -> 복사 -> 소멸 -> 해제의 비용이 들어가는 것을 방지
	//스레드 풀에서는 최대 스레드의 개수를 10개로 제한하였기에 사용 가능함
	threads.reserve(thread_num);

	//미리 정의한 멤버함수 WorkThreads를 실행하는 총 10개의 스레드를
	//스레드 벡터에 추가
	for (int i = 0; i < thread_num; ++i)
	{
		threads.emplace_back([this]() { this->WorkThread(); });
	}
}

ThreadPool::~ThreadPool()
{
	stop_all = true;
	cv_jod_q.notify_all(); //조건 변수를 기다리는 모든 스레드를 깨움. 즉, 대기하는 모든 스레드를 깨움

	//threads 벡터에 저장된 모든 요소를 접근
	for (auto& thread : threads)
	{
		thread.join(); //각 스레드가 종료될 때 까지 기다림. 스레드가 동작 중에 프로그램이 종료되면 스레드가 정상적으로 종료되지 않기 때문
	}
}

void ThreadPool::EnqueueJob(std::function<void()> job)
{
	if (stop_all)
	{
	    std::cout<< "모든 스레드가 작업을 중지하였습니다." <<std::endl;
		return;
	}

	//lock_guard는 lock 사용 후 unlock을 하지않아 발생하는 데드락 현상을 방지하기 위해 사용
	//작업 큐에 작업을 넣어야하기에 동기화를 실시
	std::lock_guard<std::mutex> lock(m_job_q);
	jobs.push(std::move(job));

	//대기 중인 스레드를 하나 깨움
	cv_jod_q.notify_one();
}

void ThreadPool::WorkThread()
{
	//스레드 수행
	while (true)
	{
		//동기화 시작
		std::unique_lock<std::mutex> lock(m_job_q);

		//작업 큐에 대기중인 작업이 남아있거나 stop_all이 설정될 때 까지 다른 스레드들이 대기
		cv_jod_q.wait(lock, [this]() { return !this->jobs.empty() || stop_all; });

		//작업 큐에 대기중인 작업이 없을 때 스레드 수행이 끝남
		if (stop_all && this->jobs.empty())
		{ 
			return;
		}

		//작업 큐에 저장된 가장 첫 번째 작업을 꺼냄
		std::function<void()> job = std::move(jobs.front());
		//가장 첫 번째 작업을 작업 큐에서 제거
		jobs.pop();

		//동기화 해제
		lock.unlock();

		//할당받은 작업을 스레드가 수행
		job();
	}
}