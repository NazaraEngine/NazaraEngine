#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Catch/catch.hpp>

SCENARIO("Clock", "[CORE][CLOCK]")
{
	GIVEN("A clock paused")
	{
		Nz::UInt64 initialTime = 100;
		Nz::Clock clock(initialTime, true);

		WHEN("We get time since it is paused")
		{
			THEN("Time must be the initialTime")
			{
				CHECK(clock.GetMicroseconds() == initialTime);
				CHECK(clock.IsPaused());
			}
		}

		WHEN("We unpause it")
		{
			clock.Unpause();
			REQUIRE(!clock.IsPaused());

			THEN("Time must not be the initialTime")
			{
				Nz::Thread::Sleep(1);
				Nz::UInt64 microSeconds = clock.GetMicroseconds();
				CHECK(microSeconds != initialTime);
				CHECK(microSeconds / 1000 <= clock.GetMilliseconds());
				CHECK(microSeconds / (1000.f * 1000.f) <= clock.GetSeconds());
			}
		}

		WHEN("We restart it")
		{
			clock.Restart();

			THEN("It is unpaused and we can pause it")
			{
				CHECK(!clock.IsPaused());
				clock.Pause();
				CHECK(clock.IsPaused());
				CHECK(clock.GetMicroseconds() != initialTime);
			}
		}
	}
}
