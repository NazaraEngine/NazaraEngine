#include <Nazara/Core/Time.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>

SCENARIO("Time", "[CORE][Time]")
{
	auto ToString = [](Nz::Time time) -> std::string
	{
		std::ostringstream ss;
		ss << time;
		return std::move(ss).str();
	};

	GIVEN("One second")
	{
		Nz::Time time = Nz::Time::Second();
		CHECK(time.AsDuration<std::chrono::seconds>() == std::chrono::seconds(1));
		CHECK(time.AsDuration<std::chrono::milliseconds>() == std::chrono::milliseconds(1'000));
		CHECK(time.AsDuration<std::chrono::microseconds>() == std::chrono::microseconds(1'000'000));
		CHECK(time.AsMicroseconds() == 1'000'000);
		CHECK(time.AsMilliseconds() == 1'000);
		CHECK(time.AsSeconds() == 1.f);
		CHECK(time.AsSeconds<int>() == 1);
		CHECK(time == Nz::Time::Second());
		CHECK(time == Nz::Time::Seconds(1.f));
		CHECK(time == Nz::Time::Milliseconds(1'000));
		CHECK(time == Nz::Time::Seconds(2) - Nz::Time::Milliseconds(1'000));

		CHECK(ToString(time) == "1000ms");
	}

	GIVEN("One arbitrary duration")
	{
		Nz::Time time = Nz::Time::FromDuration(std::chrono::nanoseconds(1'234'567'890ULL)) - Nz::Time::Nanoseconds(890);
		CHECK(time == Nz::Time::Microseconds(1'234'567));
		CHECK_FALSE(time == Nz::Time::Microseconds(1'234'568));
		CHECK(time != Nz::Time::Microseconds(1'234'566));
		CHECK_FALSE(time != Nz::Time::Microseconds(1'234'567));
		CHECK(time > Nz::Time::Microseconds(1'234'000));
		CHECK(time > Nz::Time::Zero());
		CHECK(time >= Nz::Time::Microseconds(1'234'567));
		CHECK(time >= Nz::Time::Microseconds(1'234'000));
		CHECK(time < Nz::Time::Milliseconds(1'235));
		CHECK(time <= Nz::Time::Microseconds(1'234'567));
		CHECK(time <= Nz::Time::Seconds(2));

		CHECK(ToString(time) == "1.23457s");
		CHECK(ToString(-time) == "-1.23457s");
		CHECK(ToString(time - Nz::Time::Second()) == "234.567ms");
		CHECK(ToString(Nz::Time::Second() - time) == "-234.567ms");
		CHECK(ToString(time - Nz::Time::Seconds(1.234f)) == "567us");
		CHECK(ToString(Nz::Time::Seconds(1.234f) - time) == "-567us");
	}
}
