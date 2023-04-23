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

			triggerCount++;
		});

		app.Update(Nz::Time::Milliseconds(10));
		app.Update(Nz::Time::Milliseconds(10));
		app.Update(Nz::Time::Milliseconds(10));

		CHECK(triggerCount == 3);
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
