#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>

SCENARIO("Vector2", "[MATH][VECTOR2]")
{
	GIVEN("Two same vectors (1, 1)")
	{
		Nz::Vector2f firstUnit(1.f);
		Nz::Vector2f secondUnit(Nz::Vector2i(Nz::Vector4i(1, 1, 3, 5)));

		WHEN("We compare them")
		{
			THEN("They are the same")
			{
				REQUIRE(firstUnit == secondUnit);
				REQUIRE(firstUnit <= secondUnit);
			}
		}

		WHEN("We test the dot product")
		{
			Nz::Vector2f tmp(-1.f, 1.f);

			THEN("These are perpendicular")
			{
				REQUIRE(firstUnit.AbsDotProduct(tmp) == Catch::Approx(2.f));
				REQUIRE(firstUnit.DotProduct(tmp) == Catch::Approx(0.f));
				REQUIRE(firstUnit.AngleBetween(tmp).ApproxEqual(Nz::DegreeAnglef(90.f)));
				Nz::Vector2f negativeUnitX = -Nz::Vector2f::UnitX();
				REQUIRE(negativeUnitX.AngleBetween(negativeUnitX + Nz::Vector2f(0, 0.0000001f)).ApproxEqual(Nz::DegreeAnglef(360.f)));
			}
		}

		WHEN("We ask for distance from (-2, -3)")
		{
			Nz::Vector2f tmp(-2.f, -3.f);
			Nz::Vector2f tmp2(-1.f, -1.f);

			THEN("These are expected")
			{
				REQUIRE(firstUnit.Distance(tmp2) == Catch::Approx(2.f * std::sqrt(2.f)));
				REQUIRE(firstUnit.Distance(tmp) == Catch::Approx(5.f));
				REQUIRE(firstUnit.SquaredDistance(tmp) == Catch::Approx(25.f));

				REQUIRE(firstUnit.GetSquaredLength() == Catch::Approx(2.f));
				REQUIRE(firstUnit.GetLength() == Catch::Approx(std::sqrt(2.f)));
			}
		}

		WHEN("We nomalize the vectors")
		{
			float ratio = 0.f;
			THEN("For normal cases should be normal")
			{
				Nz::Vector2f normalized = firstUnit.GetNormal(&ratio);
				REQUIRE(normalized.ApproxEqual(Nz::Vector2f::Unit() / std::sqrt(2.f)));
				REQUIRE(ratio == Catch::Approx(std::sqrt(2.f)));
			}

			THEN("For null vector")
			{
				Nz::Vector2f zero = Nz::Vector2f::Zero();
				REQUIRE(zero.GetNormal(&ratio) == Nz::Vector2f::Zero());
				REQUIRE(ratio == Catch::Approx(0.f));
			}
		}

		WHEN("We try to maximize and minimize")
		{
			Nz::Vector2f maximize(2.f, 1.f);
			Nz::Vector2f minimize(1.f, 2.f);

			THEN("The minimised and maximised should be (1, 1) and (2, 2)")
			{
				Nz::Vector2f maximized = maximize;
				Nz::Vector2f minimized = minimize;
				REQUIRE(minimized.Minimize(maximized) == Nz::Vector2f::Unit());
				REQUIRE(maximize.Maximize(minimize) == (2.f * Nz::Vector2f::Unit()));
			}

		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Vector2f zero = Nz::Vector2f::Zero();
				Nz::Vector2f unit = Nz::Vector2f::Unit();
				REQUIRE(Nz::Vector2f::Lerp(zero, unit, 0.5f) == (Nz::Vector2f::Unit() * 0.5f));
			}
		}

		WHEN("We use apply")
		{
			secondUnit.Apply([](float v) { return v - 1.f; });
			CHECK(secondUnit.ApproxEqual(Nz::Vector2f::Zero()));
		}

		WHEN("We use apply with conversion")
		{
			Nz::Vector2 vec = Nz::Vector2f::Apply(firstUnit, [](float v) -> int { return static_cast<int>(std::round(v + 1.f)); });
			CHECK(vec == Nz::Vector2i { 2, 2 });
		}
	}
}
