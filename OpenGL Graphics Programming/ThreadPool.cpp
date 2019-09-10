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


void ThreadPool::Start()
{
	for (unsigned int i = 0; i < m_iNumberOfThreads; i++)
	{
		m_workerThreads.push_back(std::thread(&ThreadPool::DoWork, this));
	}
}

void ThreadPool::Stop()
{
	m_finished = true;
}

void ThreadPool::DoWork()
{
	//Entry point of  a thread.
	std::cout << std::endl << "Thread with id " << std::this_thread::get_id() << "starting........" << std::endl;
	while (!m_finished)
	{
		std::unique_ptr<BasicTask> WorkItem;
		//If there is an item in the queue to be processed; just take it off the q and process it
		if (m_unqpWorkQueue->blocking_pop(WorkItem, 500))
		{
			std::cout << std::endl << "Thread with id " << std::this_thread::get_id() << " is working on item in the work queue" << std::endl;
			WorkItem->execute();
			std::cout << std::endl << "Thread with id " << std::this_thread::get_id() << " finished processing item " << std::endl;
			//Sleep to simulate work being done
			std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1001));
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