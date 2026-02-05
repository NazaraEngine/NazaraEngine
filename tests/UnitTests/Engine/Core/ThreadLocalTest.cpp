#include <Nazara/Core/ThreadLocalData.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <atomic>
#include <optional>
#include <thread>

std::atomic_uint s_atomicCounterCreated = 0;
std::atomic_uint s_atomicCounterDestroyed = 0;

struct AtomicCounter
{
	AtomicCounter() { s_atomicCounterCreated++; }
	~AtomicCounter() { s_atomicCounterDestroyed++; }
};

SCENARIO("ThreadLocalData", "[CORE][ThreadLocalData]")
{
	GIVEN("A Thread local data")
	{
		Nz::ThreadLocalData<AtomicCounter> threadLocalData1;
		Nz::ThreadLocalData<AtomicCounter> threadLocalData2;

		unsigned int creationCounter = s_atomicCounterCreated.load();
		unsigned int destroyedCounter = s_atomicCounterDestroyed.load();

		CHECK(s_atomicCounterDestroyed == creationCounter);
		std::thread t1([&]
		{
			CHECK(s_atomicCounterCreated == creationCounter);
			CHECK(s_atomicCounterDestroyed == destroyedCounter);
			threadLocalData1.GetOrCreate();
			CHECK(s_atomicCounterCreated == creationCounter + 1);
			CHECK(s_atomicCounterDestroyed == destroyedCounter);
			threadLocalData1.GetOrCreate();
			CHECK(s_atomicCounterCreated == creationCounter + 1);
			threadLocalData2.GetOrCreate();
			CHECK(s_atomicCounterCreated == creationCounter + 2);
			CHECK(s_atomicCounterDestroyed == destroyedCounter);
		});
		t1.join();
		CHECK(s_atomicCounterDestroyed == destroyedCounter + 2);
	}

	GIVEN("A Thread local data destroyed within a thread")
	{
		std::thread t1([&]
		{
			unsigned int creationCounter = s_atomicCounterCreated.load();
			unsigned int destroyedCounter = s_atomicCounterDestroyed.load();

			std::optional<Nz::ThreadLocalData<AtomicCounter>> threadLocalData;
			threadLocalData.emplace();

			CHECK(s_atomicCounterCreated == creationCounter);
			CHECK(s_atomicCounterDestroyed == destroyedCounter);

			threadLocalData->GetOrCreate();

			CHECK(s_atomicCounterCreated == creationCounter + 1);
			CHECK(s_atomicCounterDestroyed == destroyedCounter);

			threadLocalData.reset();

			CHECK(s_atomicCounterCreated == creationCounter + 1);
			CHECK(s_atomicCounterDestroyed == destroyedCounter + 1);
		});
		t1.join();
	}
}
