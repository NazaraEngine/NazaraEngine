#include <Nazara/Core/Clock.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

SCENARIO("Clock", "[CORE][CLOCK]")
{
	GIVEN("A clock paused")
	{
		Nz::Time initialTime = Nz::Time::Microseconds(100);
		Nz::HighPrecisionClock clock(initialTime, true);

		WHEN("We get time since it is paused")
		{
			THEN("Time must be the initialTime")
			{
				CHECK(clock.GetElapsedTime() == initialTime);
				CHECK(clock.IsPaused());
			}
		}

		WHEN("We unpause it")
		{
			clock.Unpause();
			REQUIRE(!clock.IsPaused());

			THEN("Time must not be the initialTime")
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				Nz::Time elapsedTime = clock.GetElapsedTime();
				Nz::Int64 microseconds = elapsedTime.AsMicroseconds();
				CHECK(microseconds != initialTime.AsMicroseconds());
				CHECK(microseconds / 1000 <= elapsedTime.AsMilliseconds());
				CHECK(microseconds / (1000.f * 1000.f) <= elapsedTime.AsSeconds());
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
				CHECK(clock.GetElapsedTime() != initialTime);
			}
		}

		WHEN("We restart if over")
		{
			clock.Restart(Nz::Time::Milliseconds(1'500), true);
			CHECK(clock.RestartIfOver(Nz::Time::Microseconds(1'000)) == Nz::Time::Milliseconds(1'500));
			CHECK(clock.GetElapsedTime() == Nz::Time::Zero());
		}

		WHEN("We tick it")
		{
			clock.Restart(Nz::Time::Milliseconds(1'500), true);
			CHECK(clock.Tick(Nz::Time::Milliseconds(1'000)));
			CHECK(clock.GetElapsedTime() == Nz::Time::Milliseconds(500));
			CHECK_FALSE(clock.Tick(Nz::Time::Milliseconds(1'000)));
		}
	}
}
