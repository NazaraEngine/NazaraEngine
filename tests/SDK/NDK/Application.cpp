#include <NDK/ClientApplication.hpp>
#include <Catch/catch.hpp>

SCENARIO("Application", "[NDK][APPLICATION]")
{
	GIVEN("An application")
	{
		Nz::Window& window = Ndk::ClientApplication::Instance()->AddWindow<Nz::Window>();

		WHEN("We open a window")
		{
			REQUIRE(window.Create(Nz::VideoMode(800, 600, 32), "Nazara Unit Tests"));

			AND_WHEN("We close the open window")
			{
				window.Close();

				THEN("Application should close")
				{
					REQUIRE(!Ndk::ClientApplication::Instance()->Run());
				}
			}
		}
	}
}
