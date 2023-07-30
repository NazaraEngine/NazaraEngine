#include <Nazara/Math/Plane.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Plane", "[MATH][PLANE]")
{
	GIVEN("Two planes normal(1, 1, 1), distance 1")
	{
		Nz::Planef firstPlane(Nz::Vector3f::Unit().Normalize(), 1.f);
		Nz::Planef secondPlane(Nz::Planed(Nz::Vector3d::Unit().Normalize(), 1.0));

		WHEN("We compare them")
		{
			THEN("They are equal")
			{
				REQUIRE(firstPlane == secondPlane);
			}

			AND_THEN("They have the same distance from the same point")
			{
				Nz::Vector3f point(-2.f, 3.f, 1.f);
				REQUIRE(firstPlane.SignedDistance(point) == Catch::Approx(secondPlane.SignedDistance(point)));
				REQUIRE(firstPlane.SignedDistance({ -2.f, 3.f, 1.f }) == Catch::Approx(2.1547f));
			}

			AND_THEN("Distance between Plane (0, 1, 0), distance 1 and point (0, 2, 0) should be 3")
			{
				REQUIRE(Nz::Planef(Nz::Vector3f::UnitY(), 1.f).SignedDistance(Nz::Vector3f::Zero()) == Catch::Approx(1.f));
				REQUIRE(Nz::Planef(Nz::Vector3f::UnitY(), 1.f).SignedDistance(Nz::Vector3f::UnitY() * 2.f) == Catch::Approx(3.f));
			}

			AND_THEN("Distance between Plane (0, 1, 0), distance 5 and point (0, 2, 0) should be 7")
			{
				REQUIRE(Nz::Planef(Nz::Vector3f::UnitY(), 5.f).SignedDistance(Nz::Vector3f::UnitY() * 2.f) == Catch::Approx(7.f));
			}

			AND_THEN("Distance between Plane (0, 1, 0), distance 1000 and point (0, 500, 0) and (0, 1500, 0)")
			{
				REQUIRE(Nz::Planef(Nz::Vector3f::UnitY(), 1000.f).SignedDistance(Nz::Vector3f::UnitY() * 500.f) == Catch::Approx(1500.f));
				REQUIRE(Nz::Planef(Nz::Vector3f::UnitY(), 1000.f).SignedDistance(Nz::Vector3f::UnitY() * 1500.f) == Catch::Approx(2500.f));
			}

			AND_THEN("Distance between Plane (0, -1, 0), distance -1000 and point (0, 500, 0) and (0, 1500, 0)")
			{
				REQUIRE(Nz::Planef(-Nz::Vector3f::UnitY(), -1000.f).SignedDistance(Nz::Vector3f::UnitY() * 500.f) == Catch::Approx(-1500.f));
				REQUIRE(Nz::Planef(-Nz::Vector3f::UnitY(), -1000.f).SignedDistance(Nz::Vector3f::UnitY() * 1500.f) == Catch::Approx(-2500.f));
			}
		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Planef planeXY = Nz::Planef::XY();
				Nz::Planef planeXZ = Nz::Planef::XZ();
				Nz::Vector3f result = Nz::Vector3f(0.f, 1.f, 1.f) * 0.5f;
				result.Normalize();
				REQUIRE(Nz::Planef::Lerp(planeXY, planeXZ, 0.5f) == Nz::Planef(result, 0.f));
			}
		}
	}

	GIVEN("The plane XZ, distance 1 with 3 points (0, 1, 0), (1, 1, 1), (-1, 1, 0)")
	{
		WHEN("We do a positive plane")
		{
			Nz::Planef xy(Nz::Vector3f(2.f, 1.f, 0.f), Nz::Vector3f(-1.f, 1.f, -1.f), Nz::Vector3f(-1.f, 1.f, 0.f));

			THEN("It must be equal to XZ distance 1")
			{
				REQUIRE(xy.ApproxEqual(Nz::Planef(Nz::Vector3f::UnitY(), -1.f)));
			}
		}

		WHEN("We do a negative plane")
		{
			Nz::Planef xy(Nz::Vector3f(0.f, 1.f, 0.f), Nz::Vector3f(1.f, 1.f, 1.f), Nz::Vector3f(-1.f, 1.f, 0.f));
			THEN("It must be equal to XZ distance 1")
			{
				REQUIRE(xy.ApproxEqual(Nz::Planef(-Nz::Vector3f::UnitY(), 1.f)));
			}
		}
	}
}
