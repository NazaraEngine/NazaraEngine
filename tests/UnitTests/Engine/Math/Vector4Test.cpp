#include <Nazara/Math/Vector4.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Nazara/Math/Vector3.hpp>

SCENARIO("Vector4", "[MATH][VECTOR4]")
{
	GIVEN("Two same unit vector")
	{
		Nz::Vector4f firstUnit(1.f, 1.f, 1.f);
		Nz::Vector4f secondUnit(Nz::Vector4i(1, 1, 1, 1));

		WHEN("We compare them")
		{
			THEN("They are the same")
			{
				REQUIRE(firstUnit == secondUnit);
			}
		}

		WHEN("We test the dot product")
		{
			Nz::Vector4f tmp(-1.f, 0.f, 1.f, 0.f);

			THEN("These results are expected")
			{
				REQUIRE(firstUnit.AbsDotProduct(tmp) == Catch::Approx(2.f));
				REQUIRE(firstUnit.DotProduct(tmp) == Catch::Approx(0.f));
			}
		}

		WHEN("We normalize")
		{
			Nz::Vector4f tmp(1.f, 1.f, 1.f, 3.f);

			THEN("These results are expected")
			{
				REQUIRE(firstUnit.Normalize() == Nz::Vector4f(1.f, Nz::Vector3f::Unit()));
				REQUIRE(tmp.Normalize() == Nz::Vector4f(Nz::Vector3f::Unit() * (1.f / 3.f), 1.f));
			}
		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Vector4f zero = Nz::Vector4f::Zero();
				Nz::Vector4f unitX = Nz::Vector4f::UnitX();
				REQUIRE(Nz::Vector4f::Lerp(zero, unitX, 0.5f) == Nz::Vector4f(Nz::Vector3f::UnitX() * 0.5f, 1.f));
			}
		}

		WHEN("We use apply")
		{
			secondUnit.Apply([](float v) { return v - 1.f; });
			CHECK(secondUnit.ApproxEqual(Nz::Vector4f(0.f)));
		}

		WHEN("We use apply with conversion")
		{
			Nz::Vector4 vec = Nz::Vector4f::Apply(firstUnit, [](float v) -> int { return static_cast<int>(std::round(v + 1.f)); });
			CHECK(vec == Nz::Vector4i{ 2, 2, 2, 2 });
		}
	}
}
