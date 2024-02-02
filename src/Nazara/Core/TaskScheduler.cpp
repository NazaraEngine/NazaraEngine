// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/ThreadExt.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <wsq.hpp>
#include <mutex>
#include <new>
#include <random>
#include <semaphore>
#include <thread>

#ifdef NAZARA_WITH_TSAN
#include <sanitizer/tsan_interface.h>
#endif

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

		class Spinlock
		{
			public:
				Spinlock() = default;
				Spinlock(const Spinlock&) = delete;
				Spinlock(Spinlock&&) = delete;
				~Spinlock() = default;

				void lock()
				{
					while (m_flag.test_and_set());
				}

				bool try_lock(unsigned int maxLockCount = 1)
				{
					unsigned int lockCount = 0;
					while (m_flag.test_and_set())
					{
						if (++lockCount >= maxLockCount)
							return false;
					}

					return true;
				}

				void unlock()
				{
					m_flag.clear();
				}

				Spinlock& operator=(const Spinlock&) = delete;
				Spinlock& operator=(Spinlock&&) = delete;

			private:
				std::atomic_flag m_flag;
		};
	}

	class alignas(NAZARA_ANONYMOUS_NAMESPACE_PREFIX(hardware_destructive_interference_size * 2)) TaskScheduler::Worker
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

			// "Implement" movement to make the compiler happy
			Worker(Worker&& worker) :
			m_owner(worker.m_owner)
			{
				NAZARA_UNREACHABLE();
			}

			~Worker()
			{
				m_running = false;
				if (!m_notifier.test_and_set())
					m_notifier.notify_one();

				m_thread.join();
			}

			void AddTask(TaskScheduler::Task* task)
			{
				std::unique_lock lock(m_queueSpinlock);
				{
					m_tasks.push(task);
				}
				WakeUp();
			}

			void Run()
			{
				// Wait until task scheduler started
				m_notifier.wait(false);

				StackArray<unsigned int> randomWorkerIndices = NazaraStackArrayNoInit(unsigned int, m_owner.GetWorkerCount() - 1);
				{
					unsigned int* indexPtr = randomWorkerIndices.data();
					for (unsigned int i = 0; i < m_owner.GetWorkerCount(); ++i)
					{
						if (i != m_workerIndex)
							*indexPtr++ = i;
					}

					std::minstd_rand gen(std::random_device{}());
					std::shuffle(randomWorkerIndices.begin(), randomWorkerIndices.end(), gen);
				}

				bool idle = false;
				do
				{
					// Get a task
					TaskScheduler::Task* task;
					{
						std::unique_lock lock(m_queueSpinlock);
						task = m_tasks.pop();
					}

					if (!task)
					{
						for (unsigned int workerIndex : randomWorkerIndices)
						{
							task = m_owner.GetWorker(workerIndex).StealTask();
							if (task)
								break;
						}
					}

					if (task)
					{
						if (idle)
						{
							m_owner.NotifyWorkerActive();
							idle = false;
						}

#ifdef NAZARA_WITH_TSAN
						// Workaround for TSan false-positive
						__tsan_acquire(task);
#endif

						(*task)();

#ifdef NAZARA_WITH_TSAN
						// Workaround for TSan false-positive
						__tsan_release(task);
#endif
					}
					else
					{
						// Wait for tasks if we don't have any right now
						if (!idle)
						{
							m_owner.NotifyWorkerIdle();
							idle = true;
						}

						m_notifier.wait(false);
						m_notifier.clear();
					}
				}
				while (m_running.load(std::memory_order_relaxed));
			}

			TaskScheduler::Task* StealTask()
			{
				return m_tasks.steal();
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
			NAZARA_ANONYMOUS_NAMESPACE_PREFIX(Spinlock) m_queueSpinlock;
			WorkStealingQueue<TaskScheduler::Task*, TaskScheduler::Task*> m_tasks;
			TaskScheduler& m_owner;
			unsigned int m_workerIndex;
	};

	NAZARA_WARNING_POP()

	TaskScheduler::TaskScheduler(unsigned int workerCount) :
	m_idle(false),
	m_idleWorkerCount(0),
	m_nextWorkerIndex(0),
	m_tasks(256 * sizeof(Task)),
	m_workerCount(workerCount)
	{
		if (m_workerCount == 0)
			m_workerCount = std::max(Core::Instance()->GetHardwareInfo().GetCpuThreadCount(), 1u);

		m_workers.reserve(m_workerCount);
		for (unsigned int i = 0; i < m_workerCount; ++i)
			m_workers.emplace_back(*this, i);

		for (unsigned int i = 0; i < m_workerCount; ++i)
			m_workers[i].WakeUp();

		// Wait until all worked started
		m_idle.wait(false);
	}

	TaskScheduler::~TaskScheduler()
	{
		m_workers.clear();
	}

	void TaskScheduler::AddTask(Task&& task)
	{
		m_idle = false;

		std::size_t taskIndex; //< not used
		Task* taskPtr = m_tasks.Allocate(taskIndex, std::move(task));

#ifdef NAZARA_WITH_TSAN
		// Workaround for TSan false-positive
		__tsan_release(taskPtr);
#endif

		Worker& worker = m_workers[m_nextWorkerIndex++];
		worker.AddTask(taskPtr);

		if (m_nextWorkerIndex >= m_workers.size())
			m_nextWorkerIndex = 0;
	}

	void TaskScheduler::WaitForTasks()
	{
		m_idle.wait(false);

#ifdef NAZARA_WITH_TSAN
		// Workaround for TSan false-positive
		for (Task& task : m_tasks)
			__tsan_acquire(&task);
#endif

		m_tasks.Clear();
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
