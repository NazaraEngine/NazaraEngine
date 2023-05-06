#include <Nazara/Core/Application.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Application", "[CORE][ABSTRACTHASH]")
{
	WHEN("Updating the application multiple times")
	{
		Nz::ApplicationBase app;

		std::size_t triggerCount = 0;
		app.AddUpdaterFunc([&](Nz::Time elapsedTime)
		{
			if (triggerCount == 0)
			{
				INFO("First update should have elapsed time as zero");
				CHECK(elapsedTime == Nz::Time::Zero());
			}
			else
				CHECK(elapsedTime == Nz::Time::Milliseconds(10));

			triggerCount++;
		});

		for (std::size_t i = 0; i < 10; ++i)
			app.Update(Nz::Time::Milliseconds(10));

		CHECK(triggerCount == 10);
	}
	
	WHEN("Using interval")
	{
		Nz::ApplicationBase app;

		std::size_t triggerCount = 0;
		app.AddUpdaterFunc(Nz::ApplicationBase::Interval{ Nz::Time::Milliseconds(100) }, [&](Nz::Time elapsedTime)
		{
			if (triggerCount == 0)
			{
				INFO("First update should have elapsed time as zero");
				CHECK(elapsedTime == Nz::Time::Zero());
			}
			else
			{
				INFO("Following update should have elapsed time >= interval");
				CHECK(elapsedTime >= Nz::Time::Milliseconds(100));
				CHECK(elapsedTime < Nz::Time::Milliseconds(200));
			}

			triggerCount++;
		});

		app.Update(Nz::Time::Milliseconds(100));
		CHECK(triggerCount == 1);
		app.Update(Nz::Time::Milliseconds(10));
		CHECK(triggerCount == 1);
		app.Update(Nz::Time::Milliseconds(100));
		CHECK(triggerCount == 2);
		app.Update(Nz::Time::Milliseconds(90));
		CHECK(triggerCount == 2); // this does not trigger since 100ms have not elapsed since last update
		app.Update(Nz::Time::Milliseconds(10));
		CHECK(triggerCount == 3);
		app.Update(Nz::Time::Milliseconds(100));
		CHECK(triggerCount == 4);
		for (std::size_t i = 0; i < 10; ++i)
			app.Update(Nz::Time::Milliseconds(10));
		CHECK(triggerCount == 5);
	}

	WHEN("Using fixed-time interval")
	{
		Nz::ApplicationBase app;

		std::size_t triggerCount = 0;
		app.AddUpdaterFunc(Nz::ApplicationBase::FixedInterval{ Nz::Time::Milliseconds(100) }, [&](Nz::Time elapsedTime)
		{
			CHECK(elapsedTime == Nz::Time::Milliseconds(100));
			triggerCount++;
		});

		app.Update(Nz::Time::Milliseconds(100));
		CHECK(triggerCount == 1);
		app.Update(Nz::Time::Milliseconds(10));
		CHECK(triggerCount == 1);
		app.Update(Nz::Time::Milliseconds(100));
		CHECK(triggerCount == 2);
		app.Update(Nz::Time::Milliseconds(90));
		CHECK(triggerCount == 3); // lost time is caught up
	}
}
