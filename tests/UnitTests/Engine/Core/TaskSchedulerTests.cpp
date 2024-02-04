#include <Nazara/Core/TaskScheduler.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_test_macros.hpp>
#include <atomic>
#include <chrono>
#include <random>
#include <thread>

SCENARIO("TaskScheduler", "[CORE][TaskScheduler]")
{
	for (std::size_t workerCount : { 0, 1, 2, 4, 8 })
	{
		GIVEN("A task scheduler with " << workerCount << " workers")
		{
			Nz::TaskScheduler scheduler(workerCount);

			WHEN("We add a single task and wait for it")
			{
				bool executed = false;
				scheduler.AddTask([&] { executed = true; });
				scheduler.WaitForTasks();

				CHECK(executed);
			}

			WHEN("We add time-consuming tasks, they are split between workers")
			{
				std::atomic_uint count = 0;
				for (unsigned int i = 0; i < scheduler.GetWorkerCount(); ++i)
				{
					scheduler.AddTask([&]
					{
						std::minstd_rand gen(Catch::getSeed());
						std::uniform_int_distribution dis(10, 150);
						std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
						count++;
					});
				}
				scheduler.WaitForTasks();

				CHECK(count == scheduler.GetWorkerCount());
			}

			WHEN("We add a lot of tasks and wait for all of them")
			{
				constexpr std::size_t taskCount = 512;

				std::vector<Nz::UInt8> completionBuffer(taskCount, 0);
				std::atomic_uint count = 0;
				for (std::size_t i = 0; i < taskCount; ++i)
				{
					scheduler.AddTask([&, i]
					{
						completionBuffer[i]++;
						count++;
					});
				}
				scheduler.WaitForTasks();

				unsigned int c = count.load(); //< load it once before checking to avoid race condition when testing and printing
				CHECK(c == taskCount);

				for (std::size_t i = 0; i < taskCount; ++i)
				{
					INFO("checking that task " << i << " was executed once");
					CHECK(completionBuffer[i] == 1);
				}
			}
		}
	}
}
