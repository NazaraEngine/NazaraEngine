#include <Nazara/Graphics/ColorBackground.hpp>
#include <Catch/catch.hpp>

SCENARIO("ColorBackground", "[GRAPHICS][COLORBACKGROUND]")
{
	GIVEN("A default color background")
	{
		Nz::ColorBackground colorBackground;

		WHEN("We assign it a color")
		{
			colorBackground.SetColor(Nz::Color::Red);

			THEN("We can get it")
			{
				REQUIRE(colorBackground.GetColor() == Nz::Color::Red);
			}
		}
	}
}
