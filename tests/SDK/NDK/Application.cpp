#include <NDK/Application.hpp>
#include <Catch/catch.hpp>

SCENARIO("Application", "[NDK][APPLICATION]")
{
	GIVEN("An application")
	{
		Ndk::Application application;
		application.AddWorld();
		Nz::Window& window = application.AddWindow<Nz::Window>();

		WHEN("We close the open window")
		{
			window.Close();

			THEN("Application should close")
			{
				REQUIRE(!application.Run());
			}
		}
	}
}