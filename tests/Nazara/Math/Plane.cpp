#include <Nazara/Math/Plane.hpp>
#include <catch.hpp>

SCENARIO("Plane", "[MATH][PLANE]")
{
	GIVEN("Two planes normal(1, 1, 1), distance 1")
	{
		NzPlanef firstPlane(NzVector3f::Unit().Normalize(), 1.f);
		NzPlanef secondPlane(NzPlaned(NzVector3d::Unit().Normalize(), 1.0));

		WHEN("We compare them")
		{
			THEN("They are equal")
			{
				REQUIRE(firstPlane == secondPlane);
			}

			AND_THEN("We compare with normal(-1, -1, -1), distance -1")
			{
				REQUIRE(firstPlane == NzPlanef(-NzVector3f::Unit().Normalize(), -1.f));
			}

			AND_THEN("They have the same distance from the same point")
			{
				NzVector3f point(-2.f, 3.f, 1.f);
				REQUIRE(firstPlane.Distance(point) == Approx(secondPlane.Distance(point)));
				REQUIRE(firstPlane.Distance(-2.f, 3.f, 1.f) == Approx(0.1547f));
			}

			AND_THEN("Distance between Plane (0, 1, 0), distance 1 and point (0, 2, 0) should be 1")
			{
				REQUIRE(NzPlanef(NzVector3f::UnitY(), 1.f).Distance(NzVector3f::UnitY() * 2.f) == Approx(1.f));
			}

			AND_THEN("Distance between Plane (0, 1, 0), distance 5 and point (0, 2, 0) should be -3")
			{
				REQUIRE(NzPlanef(NzVector3f::UnitY(), 5.f).Distance(NzVector3f::UnitY() * 2.f) == Approx(-3.f));
			}

			AND_THEN("Distance between Plane (0, 1, 0), distance 1000 and point (0, 500, 0) and (0, 1500, 0)")
			{
				REQUIRE(NzPlanef(NzVector3f::UnitY(), 1000.f).Distance(NzVector3f::UnitY() * 500.f) == Approx(-500.f));
				REQUIRE(NzPlanef(NzVector3f::UnitY(), 1000.f).Distance(NzVector3f::UnitY() * 1500.f) == Approx(500.f));
			}

			AND_THEN("Distance between Plane (0, -1, 0), distance -1000 and point (0, 500, 0) and (0, 1500, 0)")
			{
				REQUIRE(NzPlanef(-NzVector3f::UnitY(), -1000.f).Distance(NzVector3f::UnitY() * 500.f) == Approx(500.f));
				REQUIRE(NzPlanef(-NzVector3f::UnitY(), -1000.f).Distance(NzVector3f::UnitY() * 1500.f) == Approx(-500.f));
			}
		}
	}

	GIVEN("The plane XZ, distance 1 with 3 points (0, 1, 0), (1, 1, 1), (-1, 1, 0)")
	{
		WHEN("We do a positive plane")
		{
			NzPlanef xy(NzVector3f(2.f, 1.f, 0.f), NzVector3f(-1.f, 1.f, -1.f), NzVector3f(-1.f, 1.f, 0.f));

			THEN("It must be equal to XZ distance 1")
			{
				REQUIRE(xy == NzPlanef(NzVector3f::UnitY(), 1.f));
			}
		}

		WHEN("We do a negative plane")
		{
			NzPlanef xy(NzVector3f(0.f, 1.f, 0.f), NzVector3f(1.f, 1.f, 1.f), NzVector3f(-1.f, 1.f, 0.f));
			THEN("It must be equal to XZ distance 1")
			{
				REQUIRE(xy == NzPlanef(-NzVector3f::UnitY(), -1.f));
			}
		}
	}
}
