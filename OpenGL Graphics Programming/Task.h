#pragma once
#include <future>
class BasicTask
{
public:
	virtual ~BasicTask() {};
	virtual void execute() = 0;
};

template <typename Res>
class CTask : public BasicTask
{
public:
	CTask(std::function<Res()> func)
		: task(func) {
	}

	void execute()
	{
		task();
	}

	std::packaged_task<Res()> task;
};
