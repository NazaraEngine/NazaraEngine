#include <Nazara/Math/Vector2.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Math/Vector4.hpp>

SCENARIO("Vector2", "[MATH][VECTOR2]")
{
	GIVEN("Two same vectors (1, 1)")
	{
		NzVector2f firstUnit(1.f);
		NzVector2f secondUnit(NzVector2i(NzVector4i(1, 1, 3, 5)));

		WHEN("We compare them")
		{
			THEN("They are the same")
			{
				REQUIRE(firstUnit == secondUnit);
			}
		}

		WHEN("We test the dot product")
		{
			NzVector2f tmp(-1.f, 1.f);

			THEN("These results are expected")
			{
				REQUIRE(firstUnit.AbsDotProduct(tmp) == Approx(2.f));
				REQUIRE(firstUnit.DotProduct(tmp) == Approx(0.f));
				REQUIRE(firstUnit.AngleBetween(tmp) == Approx(90.f));
			}
		}

		WHEN("We ask for distance from (-2, -3)")
		{
			NzVector2f tmp(-2.f, -3.f);
			NzVector2f tmp2(-1.f, -1.f);

			THEN("These are expected")
			{
				REQUIRE(firstUnit.Distance(tmp2) == Approx(2.f * std::sqrt(2.f)));
				REQUIRE(firstUnit.Distance(tmp) == Approx(5.f));
				REQUIRE(firstUnit.SquaredDistance(tmp) == Approx(25.f));

				REQUIRE(firstUnit.GetSquaredLength() == Approx(2.f));
				REQUIRE(firstUnit.GetLength() == Approx(std::sqrt(2.f)));
			}
		}
	}
}
