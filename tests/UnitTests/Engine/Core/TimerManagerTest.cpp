#include <Nazara/Core/TimerManager.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>

SCENARIO("TimerManager", "[CORE][TimerManager]")
{
	Nz::TimerManager timerManager;

	int exec = 0;
	int v = 0;
	timerManager.AddImmediateTimer([&] { exec++; v = 2; });
	timerManager.AddImmediateTimer([&] { exec++; v = 1; });
	timerManager.AddTimer(Nz::Time::Milliseconds(500), [&] { exec++; v = 3; });
	timerManager.AddTimer(Nz::Time::Milliseconds(500), [&] { exec++; v = 4; });

	timerManager.Update(Nz::Time::Milliseconds(499));
	CHECK(exec == 2);
	CHECK(v == 1);

	timerManager.Update(Nz::Time::Milliseconds(1));
	CHECK(exec == 4);
	CHECK(v == 4);
}
