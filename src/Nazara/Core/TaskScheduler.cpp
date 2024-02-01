// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/ThreadExt.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <condition_variable>
#include <mutex>
#include <new>
#include <random>
#include <thread>
#include <Nazara/Core/Debug.hpp>

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

	class alignas(NAZARA_ANONYMOUS_NAMESPACE_PREFIX(hardware_destructive_interference_size)) TaskScheduler::Worker
	{
		public:
			Worker(TaskScheduler& owner, unsigned int workerIndex) :
			m_running(true),
			m_owner(owner),
			m_workerIndex(workerIndex)
			{
				m_thread = std::thread([this]
				{
					SetCurrentThreadName(fmt::format("NzWorker #{0}", m_workerIndex).c_str());
					Run();
				});
			}

			Worker(const Worker&) = delete;

			Worker(Worker&& worker) :
			m_owner(worker.m_owner)
			{
				NAZARA_UNREACHABLE();
			}

			~Worker()
			{
				m_running = false;
				m_conditionVariable.notify_one();

				m_thread.join();
			}

			bool AddTask(Task&& task)
			{
				std::unique_lock lock(m_mutex, std::defer_lock);
				if (!lock.try_lock())
					return false;

				m_tasks.push_back(std::move(task));
				lock.unlock();

				m_conditionVariable.notify_one();
				return true;
			}

			void Run()
			{
				StackArray<unsigned int> randomWorkerIndices = NazaraStackArrayNoInit(unsigned int, m_owner.GetWorkerCount() - 1);
				{
					unsigned int* indexPtr = randomWorkerIndices.data();
					for (unsigned int i = 0; i < randomWorkerIndices.size(); ++i)
					{
						if (i != m_workerIndex)
							*indexPtr++ = i;
					}

					std::minstd_rand gen(std::random_device{}());
					std::shuffle(randomWorkerIndices.begin(), randomWorkerIndices.end(), gen);
				}

				bool idle = true;
				for (;;)
				{
					std::unique_lock lock(m_mutex);

					// Wait for tasks if we don't have any right now
					if (m_tasks.empty())
					{
						if (!idle)
						{
							m_owner.NotifyWorkerIdle();
							idle = true;
						}

						m_conditionVariable.wait(lock, [this] { return !m_running || !m_tasks.empty(); });
					}

					if (!m_running)
						break;

					auto ExecuteTask = [&](TaskScheduler::Task& task)
					{
						if (idle)
						{
							m_owner.NotifyWorkerActive();
							idle = false;
						}

						task();
					};

					if (!m_tasks.empty())
					{
						TaskScheduler::Task task = std::move(m_tasks.front());
						m_tasks.erase(m_tasks.begin());

						lock.unlock();

						ExecuteTask(task);
					}
					else
					{
						lock.unlock();

						// Try to steal a task from another worker in a random order to avoid lock contention
						TaskScheduler::Task task;
						for (unsigned int workerIndex : randomWorkerIndices)
						{
							if (m_owner.GetWorker(workerIndex).StealTask(&task))
							{
								ExecuteTask(task);
								break;
							}
						}
					}

					// Note: it's possible for a thread to reach this point without executing a task (for example if another worker stole its only remaining task)
				}
			}

			bool StealTask(TaskScheduler::Task* task)
			{
				std::unique_lock lock(m_mutex, std::defer_lock);
				if (!lock.try_lock())
					return false;

				if (m_tasks.empty())
					return false;

				*task = std::move(m_tasks.front());
				m_tasks.erase(m_tasks.begin());
				return true;
			}

			Worker& operator=(const Worker& worker) = delete;

			Worker& operator=(Worker&&)
			{
				NAZARA_UNREACHABLE();
			}

		private:
			std::atomic_bool m_running;
			std::condition_variable m_conditionVariable;
			std::mutex m_mutex;
			std::thread m_thread;
			std::vector<TaskScheduler::Task> m_tasks;
			TaskScheduler& m_owner;
			unsigned int m_workerIndex;
	};

	NAZARA_WARNING_POP()

	TaskScheduler::TaskScheduler(unsigned int workerCount) :
	m_idle(true),
	m_nextWorkerIndex(0)
	{
		if (workerCount == 0)
			workerCount = std::max(Core::Instance()->GetHardwareInfo().GetCpuThreadCount(), 1u);

		m_idleWorkerCount = workerCount;

		m_workers.reserve(workerCount);
		for (unsigned int i = 0; i < workerCount; ++i)
			m_workers.emplace_back(*this, i);
	}

	TaskScheduler::~TaskScheduler()
	{
		m_workers.clear();
	}

	void TaskScheduler::AddTask(Task&& task)
	{
		m_idle = false;

		for (;;)
		{
			Worker& randomWorker = m_workers[m_nextWorkerIndex];
			if (randomWorker.AddTask(std::move(task)))
				break;

			if (++m_nextWorkerIndex >= m_workers.size())
				m_nextWorkerIndex = 0;
		}
	}

	unsigned int TaskScheduler::GetWorkerCount() const
	{
		return static_cast<unsigned int>(m_workers.size());
	}

	void TaskScheduler::WaitForTasks()
	{
		m_idle.wait(false);
	}

	auto TaskScheduler::GetWorker(unsigned int workerIndex) -> Worker&
	{
		return m_workers[workerIndex];
	}

	void TaskScheduler::NotifyWorkerActive()
	{
		m_idleWorkerCount--;
	}

	void TaskScheduler::NotifyWorkerIdle()
	{
		if (++m_idleWorkerCount == m_workers.size())
		{
			m_idle = true;
			m_idle.notify_one();
		}
	}
}
