// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/ThreadExt.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <concurrentqueue.h>
#include <mutex>
#include <new>
#include <random>
#include <semaphore>
#include <thread>

namespace Nz
{
	NAZARA_WARNING_PUSH()
	NAZARA_WARNING_MSVC_DISABLE(4324)

	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
#ifdef __cpp_lib_hardware_interference_size
		using std::hardware_destructive_interference_size;
#else
		constexpr std::size_t hardware_destructive_interference_size = 64;
#endif
	}

	struct TaskScheduler::Data
	{
		std::atomic_uint remainingTasks = 0;
		std::atomic_uint nextWorkerIndex = 0;
		std::vector<Worker> workers;
		unsigned int workerCount;
	};

	class alignas(NAZARA_ANONYMOUS_NAMESPACE_PREFIX(hardware_destructive_interference_size * 2)) TaskScheduler::Worker
	{
		public:
			Worker(TaskScheduler::Data& data, unsigned int workerIndex) :
			m_running(true),
			m_data(data),
			m_workerIndex(workerIndex)
			{
				m_thread = std::thread([this]
				{
					SetCurrentThreadName(fmt::format("NzWorker #{0}", m_workerIndex).c_str());
					Run();
				});
			}

			Worker(const Worker&) = delete;

			// "Implement" movement to make the compiler happy
			Worker(Worker&& worker) :
			m_data(worker.m_data)
			{
				NAZARA_UNREACHABLE();
			}

			~Worker() = default; // WaitForExit has to be called before destroying worker

			void AddTask(TaskScheduler::Task&& task)
			{
				m_tasks.enqueue(std::move(task));
				WakeUp();
			}

			void NotifyTaskCompletion()
			{
				if (--m_data.remainingTasks == 0)
					m_data.remainingTasks.notify_one();
			}

			void Run()
			{
				// Wait until task scheduler started
				m_notifier.wait(false);
				m_notifier.clear();

				StackArray<unsigned int> randomWorkerIndices = NazaraStackArrayNoInit(unsigned int, m_data.workerCount - 1);
				{
					unsigned int* indexPtr = randomWorkerIndices.data();
					for (unsigned int i = 0; i < m_data.workerCount; ++i)
					{
						if (i != m_workerIndex)
							*indexPtr++ = i;
					}

					std::minstd_rand gen(std::random_device{}());
					std::shuffle(randomWorkerIndices.begin(), randomWorkerIndices.end(), gen);
				}

				while (m_running.load(std::memory_order_relaxed))
				{
					// Get a task
					TaskScheduler::Task task;
					if (!m_tasks.try_dequeue(task))
					{
						for (unsigned int workerIndex : randomWorkerIndices)
						{
							task = m_data.workers[workerIndex].StealTask();
							if (task)
								break;
						}
					}

					if (task)
					{
						task();

						NotifyTaskCompletion();
					}
					else
					{
						// Wait for tasks if we don't have any right now
						m_notifier.wait(false);
						m_notifier.clear();
					}
				}
			}

			void RequestShutdown()
			{
				m_running = false;
				if (!m_notifier.test_and_set())
					m_notifier.notify_one();
			}

			TaskScheduler::Task StealTask()
			{
				TaskScheduler::Task task;
				m_tasks.try_dequeue(task);
				return task;
			}

			void WaitForExit()
			{
				m_thread.join();
			}

			void WakeUp()
			{
				if (!m_notifier.test_and_set())
					m_notifier.notify_one();
			}

			Worker& operator=(const Worker& worker) = delete;

			// "Implement" movement to make the compiler happy
			Worker& operator=(Worker&&)
			{
				NAZARA_UNREACHABLE();
			}

		private:
			std::atomic_bool m_running;
			std::atomic_flag m_notifier;
			std::thread m_thread; //< std::jthread is not yet widely implemented
			moodycamel::ConcurrentQueue<TaskScheduler::Task> m_tasks;
			TaskScheduler::Data& m_data;
			unsigned int m_workerIndex;
	};

	NAZARA_WARNING_POP()

	TaskScheduler::TaskScheduler(unsigned int workerCount)
	{
		if (workerCount == 0)
			workerCount = std::max(Core::Instance()->GetHardwareInfo().GetCpuThreadCount(), 1u);

		m_data = std::make_unique<Data>();
		m_data->workerCount = workerCount;

		m_data->workers.reserve(workerCount);
		for (unsigned int i = 0; i < workerCount; ++i)
			m_data->workers.emplace_back(*m_data, i);

		for (Worker& worker : m_data->workers)
			worker.WakeUp();
	}

	TaskScheduler::~TaskScheduler()
	{
		// Wake up workers and tell them to exit
		for (Worker& worker : m_data->workers)
			worker.RequestShutdown();

		// Wait until all threads exited before deleting workers (to avoid data-race where a worker could be freed while another tries to steal their task)
		for (Worker& worker : m_data->workers)
			worker.WaitForExit();
	}

	void TaskScheduler::AddTask(Task&& task)
	{
		m_data->remainingTasks++;

		unsigned int nextWorkerIndex = m_data->nextWorkerIndex++ % m_data->workers.size();

		Worker& worker = m_data->workers[nextWorkerIndex];
		worker.AddTask(std::move(task));
	}

	unsigned int TaskScheduler::GetWorkerCount() const
	{
		return m_data->workerCount;
	}

	void TaskScheduler::WaitForTasks()
	{
		// Wait until remaining task counter reaches 0
		for (;;) 
		{
			// Load and test current value
			unsigned int remainingTasks = m_data->remainingTasks.load();
			if (remainingTasks == 0)
				break;

			// If task count isn't 0, wait until it's signaled
			// (we need to retest remainingTasks because a worker can signal m_remainingTasks while we're still adding tasks)
			m_data->remainingTasks.wait(remainingTasks);
		}
	}
}
