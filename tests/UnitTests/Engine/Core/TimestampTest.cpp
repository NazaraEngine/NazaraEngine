#include <Nazara/Core/Timestamp.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <thread>

SCENARIO("Timestamp", "[CORE][Timestamp]")
{
	auto ToString = [](Nz::Timestamp time) -> std::string
	{
		std::ostringstream ss;
		ss << time;
		return std::move(ss).str();
	};

	GIVEN("Current timestamp")
	{
		Nz::Timestamp now = Nz::Timestamp::Now();
		CHECK(now == now);
		CHECK(now >= now);
		CHECK(now <= now);
		CHECK(now > Nz::Timestamp::FromSeconds(1724506946ll));
		CHECK(now > Nz::Timestamp::Epoch());

		std::this_thread::sleep_for(std::chrono::seconds(1));

		Nz::Timestamp future = Nz::Timestamp::Now();
		CHECK(future > now);
		CHECK(now < future);
		CHECK(future > Nz::Timestamp::Epoch());

		// Sleep is very imprecise on CI so give it a nice range
		CHECK(future - now >= Nz::Time::Milliseconds(800));
		CHECK(future - now < Nz::Time::Milliseconds(2500));
	}

	GIVEN("Arbitrary timestamp")
	{
		Nz::Timestamp time = Nz::Timestamp::FromNanoseconds(1724507141322341000ll);
		CHECK(ToString(time) == "timestamp: 1724507141.322");
#if __cpp_lib_chrono >= 201907L
		CHECK(Nz::Timestamp::FromTimepoint(time.AsTimepoint<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>>()) == time);
		CHECK(Nz::Timestamp::FromTimepoint(time.AsTimepoint<std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>>()) == time);
#endif
	}
}
