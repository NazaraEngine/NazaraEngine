#include <Nazara/Core/Color.hpp>
#include <Catch/catch.hpp>

const float epsilon = 0.01f;

void CompareColor(const Nz::Color& lhs, const Nz::Color& rhs)
{
	Nz::UInt8 tolerance = 3;
	REQUIRE(Nz::NumberEquals(lhs.r, rhs.r, tolerance));
	REQUIRE(Nz::NumberEquals(lhs.g, rhs.g, tolerance));
	REQUIRE(Nz::NumberEquals(lhs.b, rhs.b, tolerance));
	REQUIRE(Nz::NumberEquals(lhs.a, rhs.a, tolerance));
}

void CompareCMY(const Nz::Color& color, float cyan, float magenta, float yellow)
{
	float retrievedCyan = 0.f, retrievedMagenta = 0.f, retrievedYellow = 0.f;
	Nz::Color::ToCMY(color, &retrievedCyan, &retrievedMagenta, &retrievedYellow);
	CHECK(retrievedCyan == Approx(cyan).epsilon(epsilon));
	CHECK(retrievedMagenta == Approx(magenta).epsilon(epsilon));
	CHECK(retrievedYellow == Approx(yellow).epsilon(epsilon));
}

void CompareCMYK(const Nz::Color& color, float cyan, float magenta, float yellow, float black)
{
	float retrievedCyan = 0.f, retrievedMagenta = 0.f, retrievedYellow = 0.f, retrievedBlack = 0.f;
	Nz::Color::ToCMYK(color, &retrievedCyan, &retrievedMagenta, &retrievedYellow, &retrievedBlack);
	CHECK(retrievedCyan == Approx(cyan).epsilon(epsilon));
	CHECK(retrievedMagenta == Approx(magenta).epsilon(epsilon));
	CHECK(retrievedYellow == Approx(yellow).epsilon(epsilon));
	CHECK(retrievedBlack == Approx(black).epsilon(epsilon));
}

void CompareHSL(const Nz::Color& color, float hue, float saturation, float luminosity)
{
	float retrievedHue = 0.f, retrievedSaturation = 0.f, retrievedLuminosity = 0.f;
	Nz::Color::ToHSL(color, &retrievedHue, &retrievedSaturation, &retrievedLuminosity);
	CHECK(retrievedHue == Approx(hue).epsilon(epsilon));
	CHECK(retrievedSaturation == Approx(saturation).epsilon(epsilon));
	CHECK(retrievedLuminosity == Approx(luminosity).epsilon(epsilon));
}

void CompareHSV(const Nz::Color& color, float hue, float saturation, float value)
{
	float retrievedHue = 0.f, retrievedSaturation = 0.f, retrievedValue = 0.f;
	Nz::Color::ToHSV(color, &retrievedHue, &retrievedSaturation, &retrievedValue);
	CHECK(retrievedHue == Approx(hue).epsilon(epsilon));
	CHECK(retrievedSaturation == Approx(saturation).epsilon(epsilon));
	CHECK(retrievedValue == Approx(value).epsilon(epsilon));
}

void CompareXYZ(const Nz::Color& color, float x, float y, float z)
{
	Nz::Vector3f retrievedValues = Nz::Vector3f::Zero();
	Nz::Color::ToXYZ(color, &retrievedValues);
	CHECK(retrievedValues.x == Approx(x).epsilon(epsilon));
	CHECK(retrievedValues.y == Approx(y).epsilon(epsilon));
	CHECK(retrievedValues.z == Approx(z).epsilon(epsilon));
}

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

	GIVEN("A special color in different formats")
	{
		struct ColorData
		{
			const char* name;
			Nz::Color rgb;
			float cyan, magenta, yellow;
			float cyanK, magentaK, yellowK, black;
			float hue, saturation, luminosity;
			float hueV, saturationV, valueV;
			float x, y, z;
		};

		std::vector<ColorData> colors;

		colors.push_back({
			"blue",
			Nz::Color(0, 0, 255),
			1.f, 1.f, 0.f, // cmy
			1.f, 1.f, 0.f, 0.f, // cmyk
			240.f, 1.f, 0.5f, // hsl
			240.f, 1.f, 1.f, // hsv
			18.05f, 7.22f, 95.05f // xyz
		});

		colors.push_back({
			"white",
			Nz::Color(255, 255, 255),
			0.f, 0.f, 0.f, // cmy
			0.f, 0.f, 0.f, 0.f, // cmyk
			0.f, 0.f, 1.f, // hsl
			0.f, 0.f, 1.f, // hsv
			95.05f, 100.f, 108.09f // xyz
		});

		colors.push_back({
			"greenish",
			Nz::Color(5, 191, 25),
			0.980f, 0.251f, 0.902f, // cmy
			0.974f, 0.000f, 0.869f, 0.251f, // cmyk
			126.f, 0.95f, 0.38f, // hsl
			126.f, 0.97f, 0.75f, // hsv
			18.869f, 37.364f, 7.137f // xyz
		});

		for (const ColorData& color : colors)
		{
			WHEN("We perform conversion for: " + color.name)
			{
				THEN("From other color spaces")
				{
					CompareColor(color.rgb, Nz::Color::FromCMY(color.cyan, color.magenta, color.yellow));
					CompareColor(color.rgb, Nz::Color::FromCMYK(color.cyanK, color.magentaK, color.yellowK, color.black));
					CompareColor(color.rgb, Nz::Color::FromHSL(color.hue, color.saturation, color.luminosity));
					CompareColor(color.rgb, Nz::Color::FromHSV(color.hueV, color.saturationV, color.valueV));
					CompareColor(color.rgb, Nz::Color::FromXYZ(Nz::Vector3f(color.x, color.y, color.z)));
				}

				THEN("To other color spaces")
				{
					CompareCMY(color.rgb, color.cyan, color.magenta, color.yellow);
					CompareCMYK(color.rgb, color.cyanK, color.magentaK, color.yellowK, color.black);
					CompareHSL(color.rgb, color.hue, color.saturation, color.luminosity);
					CompareHSV(color.rgb, color.hueV, color.saturationV, color.valueV);
					CompareXYZ(color.rgb, color.x, color.y, color.z);
				}
			}
		}
	}
}
