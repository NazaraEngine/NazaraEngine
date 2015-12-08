#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Catch/catch.hpp>

SCENARIO("Clock", "[CORE][CLOCK]")
{
	GIVEN("A clock paused")
	{
		Nz::UInt64 initialTime = 1;
		Nz::Clock clock(initialTime, true);

		WHEN("We get time")
		{
			THEN("Time must be the initialTime")
			{
				REQUIRE(clock.GetMicroseconds() == initialTime);
			}

			AND_WHEN("We unpause it")
			{
				clock.Unpause();
				THEN("Time must not be the initialTime")
				{
					Nz::Thread::Sleep(1);
					REQUIRE(clock.GetMicroseconds() != initialTime);
				}
			}
		}
	}
}
