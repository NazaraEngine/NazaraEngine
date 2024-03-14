#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Vector3", "[MATH][VECTOR3]")
{
	GIVEN("Two same unit vector")
	{
		Nz::Vector3f firstUnit(1.f, 1.f, 1.f);
		Nz::Vector3f secondUnit(Nz::Vector3i(Nz::Vector4i(1, 1, 1, 5)));

		WHEN("We compare them")
		{
			THEN("They are the same")
			{
				REQUIRE(firstUnit == secondUnit);
			}
		}

		WHEN("We test the dot product")
		{
			Nz::Vector3f tmp(-1.f, 0.f, 1.f);

			THEN("These results are expected")
			{
				REQUIRE(firstUnit.AbsDotProduct(tmp) == Catch::Approx(2.f));
				REQUIRE(firstUnit.DotProduct(tmp) == Catch::Approx(0.f));
				REQUIRE(firstUnit.AngleBetween(tmp).ApproxEqual(Nz::DegreeAnglef(90.f)));
				REQUIRE(firstUnit.AngleBetween(-firstUnit).ApproxEqual(Nz::DegreeAnglef(180.f), 0.001f));
			}
		}

		WHEN("We test the cross product")
		{
			THEN("These results are expected")
			{
				REQUIRE(Nz::Vector3f::CrossProduct(Nz::Vector3f::UnitX(), Nz::Vector3f::UnitY()) == Nz::Vector3f::UnitZ());
				REQUIRE(Nz::Vector3f::CrossProduct(Nz::Vector3f(1.f, 2.f, 3.f), Nz::Vector3f(3.f, 2.f, 1.f)) == Nz::Vector3f(-4.f, 8.f, -4.f));
			}
		}

		WHEN("We ask for distance")
		{
			Nz::Vector3f tmp(-1.f, -5.f, -8.f);

			THEN("These are expected")
			{
				REQUIRE(firstUnit.Distance(tmp) == Catch::Approx(11.f));
				REQUIRE(firstUnit.SquaredDistance(tmp) == Catch::Approx(121.f));

				REQUIRE(firstUnit.GetSquaredLength() == Catch::Approx(3.f));
				REQUIRE(firstUnit.GetLength() == Catch::Approx(std::sqrt(3.f)));
			}
		}

		WHEN("We nomalize the vectors")
		{
			float ratio = 0.f;
			THEN("For normal cases should be normal")
			{
				Nz::Vector3f normalized = firstUnit.GetNormal(&ratio);
				REQUIRE(normalized.ApproxEqual(Nz::Vector3f::Unit() / std::sqrt(3.f)));
				REQUIRE(ratio == Catch::Approx(std::sqrt(3.f)));
			}

			THEN("For null vector")
			{
				Nz::Vector3f zero = Nz::Vector3f::Zero();
				REQUIRE(zero.GetNormal(&ratio) == Nz::Vector3f::Zero());
				REQUIRE(ratio == Catch::Approx(0.f));
			}
		}

		WHEN("We try to maximize and minimize")
		{
			Nz::Vector3f maximize(2.f, 1.f, 2.f);
			Nz::Vector3f minimize(1.f, 2.f, 1.f);

			THEN("The minimised and maximised should be (1, 1, 1) and (2, 2, 2)")
			{
				Nz::Vector3f maximized = maximize;
				Nz::Vector3f minimized = minimize;
				REQUIRE(minimized.Minimize(maximized) == Nz::Vector3f::Unit());
				REQUIRE(maximize.Maximize(minimize) == (2.f * Nz::Vector3f::Unit()));
			}

		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Vector3f zero = Nz::Vector3f::Zero();
				Nz::Vector3f unit = Nz::Vector3f::Unit();
				REQUIRE(Nz::Vector3f::Lerp(zero, unit, 0.5f) == (Nz::Vector3f::Unit() * 0.5f));
			}
		}

		WHEN("We use apply")
		{
			secondUnit.Apply([](float v) { return v - 1.f; });
			CHECK(secondUnit.ApproxEqual(Nz::Vector3f::Zero()));
		}

		WHEN("We use apply with conversion")
		{
			Nz::Vector3 vec = Nz::Vector3f::Apply(firstUnit, [](float v) -> int { return static_cast<int>(std::round(v + 1.f)); });
			CHECK(vec == Nz::Vector3i{ 2, 2, 2 });
		}
	}

	GIVEN("Two vectors")
	{
		Nz::Vector2f unit = Nz::Vector2f::Unit();
		Nz::Vector3f smaller(-1.f, unit);

		Nz::Vector3f bigger(1.f, unit.x, unit.y);

		WHEN("We combine divisions and multiplications")
		{
			Nz::Vector3f result = smaller / bigger;
			result *= bigger;

			THEN("We should get the identity")
			{
				REQUIRE(result == smaller);
			}
		}
	}
}
