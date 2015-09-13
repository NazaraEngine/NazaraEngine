#include <Nazara/Math/Sphere.hpp>
#include <catch.hpp>

SCENARIO("Sphere", "[MATH][SPHERE]")
{
	GIVEN("Two same sphere center and unit")
	{
		NzSpheref firstCenterAndUnit(0.f, 0.f, 0.f, 1.f);
		NzSpheref secondCenterAndUnit(NzSphere<int>(NzVector3i::Zero(), 1));

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
				CHECK(firstCenterAndUnit.Contains(NzBoxf(NzVector3f::Zero(), NzVector3f::Unit() * 0.5f)));
				CHECK(!firstCenterAndUnit.Contains(NzBoxf(NzVector3f::Zero(), NzVector3f::Unit() * 5.f)));
			}

			THEN("There are for Intersect")
			{
				CHECK(firstCenterAndUnit.Intersect(NzBoxf(NzVector3f::Zero(), NzVector3f::Unit() * 0.5f)));
				CHECK(firstCenterAndUnit.Intersect(NzBoxf(NzVector3f::Zero(), NzVector3f::Unit() * 5.f)));
				CHECK(!firstCenterAndUnit.Intersect(NzBoxf(NzVector3f::Unit() * 5.f, NzVector3f::Unit())));

				CHECK(firstCenterAndUnit.Intersect(NzSpheref(NzVector3f::Zero(), 0.5f)));
				CHECK(firstCenterAndUnit.Intersect(NzSpheref(NzVector3f::Zero(), 5.f)));
				CHECK(!firstCenterAndUnit.Intersect(NzSpheref(NzVector3f::Unit() * 5.f, 1.f)));
			}
		}

		WHEN("We ask for distance")
		{
			THEN("These results are expected")
			{
				REQUIRE(firstCenterAndUnit.Distance(NzVector3f::UnitX()) == Approx(1.f));
				REQUIRE(firstCenterAndUnit.SquaredDistance(NzVector3f::UnitX()) == Approx(1.f));

				NzSpheref tmp(NzVector3f::UnitX(), 1.f);
				REQUIRE(tmp.Distance(NzVector3f::UnitX() * 4.f) == Approx(3.f));
				REQUIRE(tmp.SquaredDistance(NzVector3f::UnitX() * 4.f) == Approx(9.f));
			}
		}

		WHEN("We get sphere from box unit and center")
		{
			NzBoxf centerUnitBox(NzVector3f::Unit() * -0.5f, NzVector3f::Unit() * 0.5f);

			THEN("This is equal to sphere center and radius 0.75")
			{
				REQUIRE(centerUnitBox.GetSquaredBoundingSphere() == NzSpheref(NzVector3f::Zero(), 0.75f));
			}
		}
	}
}
