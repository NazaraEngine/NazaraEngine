#include <Nazara/Core/Signal.hpp>
#include <catch2/catch.hpp>

struct Incrementer
{
	void increment(int* inc)
	{
		*inc += 1;
	}
};

void increment(int* inc)
{
	*inc += 1;
}

SCENARIO("Signal", "[CORE][SIGNAL]")
{
	GIVEN("A signal")
	{
		Nz::Signal<int*> signal;

		WHEN("We connection different callbacks")
		{
			auto connection = signal.Connect(increment);
			signal.Connect([](int* inc){ *inc += 1; });
			Incrementer incrementer;
			signal.Connect(incrementer, &Incrementer::increment);

			THEN("The call of signal with inc = 0 must return 3")
			{
				int inc = 0;
				signal(&inc);
				REQUIRE(inc == 3);
			}

			AND_THEN("When we disconnect one function, there should be only two listeners")
			{
				connection.Disconnect();
				REQUIRE(!connection.IsConnected());

				int inc = 0;
				signal(&inc);
				REQUIRE(inc == 2);
			}
		}
	}
}
