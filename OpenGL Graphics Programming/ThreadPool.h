#pragma once
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__
#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include <future>

#include "WorkQueue.h"
#include "Task.h"

class ThreadPool
{
public:
	~ThreadPool();

	// Singleton Methods
	static ThreadPool& GetInstance();
	static void DestroyInstance();

	template <typename Fun, typename ... Args>
	std::future<std::result_of_t<Fun(Args...)>> Submit(Fun func, Args... args);

	void DoWork();
	void Start(int _threadNum);
	void Stop();
	std::atomic_int& getItemsProcessed();

private:
	// Make the threadpool a singleton
	ThreadPool();
	ThreadPool(unsigned int _size);

	//The ThreadPool is non-copyable.
	ThreadPool(const ThreadPool& _kr) = delete;
	ThreadPool& operator= (const ThreadPool& _kr) = delete;

protected:
	static ThreadPool* s_pThreadPool;

private:
	//An atomic boolean variable to indicate when all the work items in the queue have been finished.
	std::atomic_bool m_finished;

	//A WorkQueue of tasks which are packaged tasks
	std::unique_ptr<CWorkQueue<std::unique_ptr<BasicTask>> > m_unqpWorkQueue;

	//Create a pool of worker threads
	std::vector<std::thread> m_workerThreads;

	//A variable to hold the number of threads we want
	unsigned int m_iNumberOfThreads;

	//An atomic variable to keep track of how many items have been processed.
	std::atomic_int m_aiItemsProcessed{ 0 };

};

template <typename Fun, typename ... Args>
std::future<std::result_of_t<Fun(Args...)>>
ThreadPool::Submit(Fun func, Args... args)
{
	using ResultT = std::result_of_t<Fun(Args...)>;
	std::unique_ptr<CTask<ResultT>> 
		task(std::make_unique<CTask<ResultT>>(std::bind(func, args...)));
	std::future<ResultT> f = task->task.get_future();
	m_unqpWorkQueue->push(std::move(task));
	return f;
}
#endif