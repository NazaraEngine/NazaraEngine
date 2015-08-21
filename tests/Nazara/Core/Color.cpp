#include <Nazara/Core/Color.hpp>
#include <catch.hpp>

SCENARIO("Color", "[CORE][COLOR]")
{
	GIVEN("Two colors, one red (255) and one gray (128)")
	{
		NzColor red(255, 0, 0);
		NzColor grey(128);

		WHEN("We do operations")
		{
			THEN("These results are expected")
			{
				red += NzColor(0, 0, 0);
				grey *= NzColor(255);
				REQUIRE((red + grey) == NzColor(255, 128, 128));
				REQUIRE((red * grey) == NzColor(128, 0, 0));
			}
		}
	}
}
