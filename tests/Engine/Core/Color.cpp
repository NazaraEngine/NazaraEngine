#include <Nazara/Core/Color.hpp>
#include <Catch/catch.hpp>

SCENARIO("Color", "[CORE][COLOR]")
{
	GIVEN("Two colors, one red (255) and one gray (128)")
	{
		Nz::Color red(255, 0, 0);
		Nz::Color grey(128);

		WHEN("We do operations")
		{
			THEN("These results are expected")
			{
				red += Nz::Color(0, 0, 0);
				grey *= Nz::Color(255);
				REQUIRE((red + grey) == Nz::Color(255, 128, 128));
				REQUIRE((red * grey) == Nz::Color(128, 0, 0));
			}
		}
	}
}
