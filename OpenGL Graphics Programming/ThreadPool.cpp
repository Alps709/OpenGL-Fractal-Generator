#include "ThreadPool.h"

//Static Variables
ThreadPool* ThreadPool::s_pThreadPool = nullptr;

ThreadPool::ThreadPool()
	:m_unqpWorkQueue(new CWorkQueue<std::unique_ptr<BasicTask>>())
{
	m_finished = false;   //Set out finished to false to begin
	m_iNumberOfThreads = std::thread::hardware_concurrency();
}

ThreadPool::ThreadPool(unsigned int _size)
{
	//Create a pool of threads equal to specified size
	m_iNumberOfThreads = _size;
	m_finished = false;   //Set out finished to false to begin
}

ThreadPool::~ThreadPool()
{
	m_finished = true;
	if (m_workerThreads.size() != 0)
	{
		for (unsigned int i = 0; i < m_iNumberOfThreads; i++)
		{

			m_workerThreads[i].join();
		}
	}
}

ThreadPool& ThreadPool::GetInstance()
{
	if (s_pThreadPool == nullptr)
	{
		s_pThreadPool = new ThreadPool();
	}
	return (*s_pThreadPool);
}

void ThreadPool::DestroyInstance()
{
	delete s_pThreadPool;
	s_pThreadPool = 0;
}

//This function is to only be called once per threadpool
void ThreadPool::Start(int _threadNum)
{
	m_iNumberOfThreads = _threadNum;
	m_workerThreads.clear();
	for (unsigned int i = 0; i < m_iNumberOfThreads; i++)
	{
		m_workerThreads.push_back(std::thread(&ThreadPool::DoWork, this));
	}
}

void ThreadPool::Stop()
{
	m_finished = true;
	DestroyInstance();
}

void ThreadPool::DoWork()
{
	//Entry point of  a thread.
	while (!m_finished)
	{
		std::unique_ptr<BasicTask> WorkItem;
		//If there is an item in the queue to be processed; just take it off the q and process it
		if (m_unqpWorkQueue->blocking_pop(WorkItem, 500))
		{
			WorkItem->execute();
			m_aiItemsProcessed++;
		}
		else {
			continue;
		}
	}
}

std::atomic_int& ThreadPool::getItemsProcessed()
{
	return m_aiItemsProcessed;
}