#include <Nazara/Math/Vector3.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector4.hpp>

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
				REQUIRE(firstUnit.AbsDotProduct(tmp) == Approx(2.f));
				REQUIRE(firstUnit.DotProduct(tmp) == Approx(0.f));
				REQUIRE(firstUnit.AngleBetween(tmp) == Approx(90.f));
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
				REQUIRE(firstUnit.Distance(tmp) == Approx(11.f));
				REQUIRE(firstUnit.SquaredDistance(tmp) == Approx(121.f));

				REQUIRE(firstUnit.GetSquaredLength() == Approx(3.f));
				REQUIRE(firstUnit.GetLength() == Approx(std::sqrt(3.f)));
			}
		}
	}
}
