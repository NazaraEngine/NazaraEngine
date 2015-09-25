#include <Nazara/Math/Vector4.hpp>
#include <Catch/catch.hpp>

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
				REQUIRE(firstUnit.AbsDotProduct(tmp) == Approx(2.f));
				REQUIRE(firstUnit.DotProduct(tmp) == Approx(0.f));
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
	}
}
