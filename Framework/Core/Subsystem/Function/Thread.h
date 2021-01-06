#pragma once
#include "../ISubsystem.h"
#include <mutex>
#include <queue>

// 구조가 뭐냐면 class Thread 에 std::thread 를 최대 갯수만큼 넣고 정지시킨다.
// 그리고 Task 를 넣어서 그때마다 std::thread 를 풀어준다.

namespace Framework
{

	class Task final
	{
	public:
		typedef std::function<void()> Process;

	public:
		Task(Process&& process) { this->process = std::forward<Process>(process); };
		~Task() = default;

		void Execute() { process(); }

	private:
		Process process;
	};

	class Thread final : public ISubsystem
	{
	public:
		Thread(class Context* context);
		~Thread();

		bool Init() override;
		void Update() override;

		void Invoke();
		int LeftJobs() { return nRunningTask; }

		template <typename Process> void AddTask(Process&& process);

	private:
		std::vector<std::thread> threads;
		std::queue<std::shared_ptr<Task>> tasks;
		std::mutex taskMutex;
		std::condition_variable conditionVar;
		uint threadCount;
		uint nRunningTask = 0;
		bool bStopping;
	};

	// thread 에서 부모의 thread 는 호출 불가능함
	template<typename Process>
	inline void Thread::AddTask(Process&& process)
	{
		if (threads.empty())
		{
			LOG_WARNING("No available thread");
			process();
			return;
		}

		std::unique_lock<std::mutex> lock(taskMutex);
		{
			tasks.push(std::make_shared<Task>(std::bind(std::forward<Process>(process))));
			nRunningTask++;
		}
		lock.unlock();

		conditionVar.notify_one();
	}
}