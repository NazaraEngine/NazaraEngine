#include <Nazara/Math/Sphere.hpp>
#include <Catch/catch.hpp>

SCENARIO("Sphere", "[MATH][SPHERE]")
{
	GIVEN("Two same sphere center and unit")
	{
		Nz::Spheref firstCenterAndUnit(0.f, 0.f, 0.f, 1.f);
		Nz::Spheref secondCenterAndUnit(Nz::Sphere<int>(Nz::Vector3i::Zero(), 1));

		WHEN("We compare them")
		{
			THEN("They are the same")
			{
				REQUIRE(firstCenterAndUnit == secondCenterAndUnit);
			}
		}

		WHEN("We ask if they intersect or contain")
		{
			THEN("These results are expected for Contains")
			{
				CHECK(firstCenterAndUnit.Contains(0.5f, 0.5f, 0.5f));
				CHECK(firstCenterAndUnit.Contains(Nz::Boxf(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 0.5f)));
				CHECK(!firstCenterAndUnit.Contains(Nz::Boxf(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 5.f)));
			}

			THEN("There are for Intersect")
			{
				CHECK(firstCenterAndUnit.Intersect(Nz::Boxf(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 0.5f)));
				CHECK(firstCenterAndUnit.Intersect(Nz::Boxf(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 5.f)));
				CHECK(!firstCenterAndUnit.Intersect(Nz::Boxf(Nz::Vector3f::Unit() * 5.f, Nz::Vector3f::Unit())));

				CHECK(firstCenterAndUnit.Intersect(Nz::Spheref(Nz::Vector3f::Zero(), 0.5f)));
				CHECK(firstCenterAndUnit.Intersect(Nz::Spheref(Nz::Vector3f::Zero(), 5.f)));
				CHECK(!firstCenterAndUnit.Intersect(Nz::Spheref(Nz::Vector3f::Unit() * 5.f, 1.f)));
			}
		}

		WHEN("We ask for distance")
		{
			THEN("These results are expected")
			{
				REQUIRE(firstCenterAndUnit.Distance(Nz::Vector3f::UnitX()) == Approx(1.f));
				REQUIRE(firstCenterAndUnit.SquaredDistance(Nz::Vector3f::UnitX()) == Approx(1.f));

				Nz::Spheref tmp(Nz::Vector3f::UnitX(), 1.f);
				REQUIRE(tmp.Distance(Nz::Vector3f::UnitX() * 4.f) == Approx(3.f));
				REQUIRE(tmp.SquaredDistance(Nz::Vector3f::UnitX() * 4.f) == Approx(9.f));
			}
		}

		WHEN("We get sphere from box unit and center")
		{
			Nz::Boxf centerUnitBox(Nz::Vector3f::Unit() * -0.5f, Nz::Vector3f::Unit() * 0.5f);

			THEN("This is equal to sphere center and radius 0.75")
			{
				REQUIRE(centerUnitBox.GetSquaredBoundingSphere() == Nz::Spheref(Nz::Vector3f::Zero(), 0.75f));
			}
		}
	}
}
