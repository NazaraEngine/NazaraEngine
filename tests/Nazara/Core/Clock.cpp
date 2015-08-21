#include <Nazara/Core/Clock.hpp>
#include <catch.hpp>

#include <thread>

SCENARIO("Clock", "[CORE][CLOCK]")
{
	GIVEN("A clock paused")
	{
		nzUInt64 initialTime = 1;
		NzClock clock(initialTime, true);

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
					std::this_thread::sleep_for(std::chrono::microseconds(10));
					REQUIRE(clock.GetMicroseconds() != initialTime);
				}
			}
		}
	}
}
