#include <Nazara/Core/TaskScheduler.hpp>
#include <catch2/catch_test_macros.hpp>
#include <atomic>

SCENARIO("TaskScheduler", "[CORE][TaskScheduler]")
{
	for (std::size_t workerCount : { 0, 1, 2, 4 })
	{
		GIVEN("A task scheduler with " << workerCount << " workers")
		{
			Nz::TaskScheduler scheduler(4);

			WHEN("We add a single task and wait for it")
			{
				bool executed = false;
				scheduler.AddTask([&] { executed = true; });
				scheduler.WaitForTasks();

				CHECK(executed);
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
