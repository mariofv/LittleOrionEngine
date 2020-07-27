#ifndef _THREADSAFEQUEUE_H_
#define _THREADSAFEQUEUE_H_

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() = default;
	~ThreadSafeQueue() = default;

	ThreadSafeQueue(const ThreadSafeQueue& copy)
	{
		std::lock_guard<std::mutex> lock(mutex);
		data_queue = copy.data_queue;
	}

	ThreadSafeQueue(const ThreadSafeQueue&& queue)
	{
		std::lock_guard<std::mutex> lock(mutex);
		data_queue = std::move(queue);
	}

	void Push(T value)
	{
		std::lock_guard<std::mutex> lock(mutex);
		data_queue.push(value);
		data_condition_variable.notify_one();
	}

	void WaitAndPop(T& value)
	{
		std::unique_lock<std::mutex>::lock(mutex);
		data_condition_variable.wait([&data_queue]() {return !data_queue.empty(); });

		value = data_queue.front();
		data_queue.pop();
	}

	bool TryPop(T& value)
	{
		std::lock_guard<std::mutex> lock(mutex);
		if(data_queue.empty())
		{
			return false;
		}

		value = data_queue.front();
		data_queue.pop();

		return true;
	}


	std::shared_ptr<T> TryPop()
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (data_queue.empty())
		{
			return nullptr;
		}

		std::shared_ptr<T> value(std::make_shared(data_queue.front()));
		data_queue.pop();

		return value;
	}

	bool Empty()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return data_queue.empty();
	}

	unsigned int Size()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return static_cast<unsigned int>(data_queue.size());
	}


private:
	std::queue<T> data_queue;
	std::mutex mutex;
	std::condition_variable data_condition_variable;

};

#endif _THREADSAFEQUEUE_H_