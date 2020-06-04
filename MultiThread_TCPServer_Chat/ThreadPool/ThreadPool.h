#pragma once
#include <thread>
#include <functional>
#include <queue>
#include <vector>
#include <future>

//스레드 풀(싱글톤 구현)
//https://modoocode.com/285 참고
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
    
	//작업 큐에 작업(job)을 추가하는 함수
	//서버나 클라에서 데이터를 주고 받는 함수는 반환값이 모두 void이기 때문에 void()를 매개변수로 가짐
	void EnqueueJob(std::function<void()> job);

private:
	//스레드 동작
	void WorkThread();

    //총 스레드의 개수
    int thread_num;
	//스레드를 보관하는 벡터
	std::vector<std::thread> threads;
	//할 일들(job)을 보관하는 큐
	std::queue<std::function<void()>> jobs;
	//큐는 멀티 스레드 환경에서는 안전하지 않기 때문에 큐의 작업처리를 관리할 보호장치
	std::condition_variable cv_jod_q;
	//뮤텍스 동기화를 사용하기 위한 변수
	std::mutex m_job_q;

	//모든 스레드 종료
	bool stop_all;
};
