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
			THEN("These results are expected because we don't take into account the border")
			{
				CHECK(firstCenterAndUnit.Distance(Nz::Vector3f::UnitX() * 2.f) == Approx(1.f));

				Nz::Spheref tmp(Nz::Vector3f::UnitX(), 1.f);
				CHECK(tmp.Distance(Nz::Vector3f::UnitX() * 4.f) == Approx(2.f));
			}
		}

		WHEN("We get sphere from box unit and center")
		{
			Nz::Boxf centerUnitBox(Nz::Vector3f::Unit() * -0.5f, Nz::Vector3f::Unit() * 0.5f);

			THEN("This is equal to sphere center and radius 0.75")
			{
				CHECK(centerUnitBox.GetSquaredBoundingSphere() == Nz::Spheref(Nz::Vector3f::Zero(), 0.75f));
			}
		}

		WHEN("We ask for positive and negative vertex")
		{
			Nz::Vector3f positiveVector = Nz::Vector3f::UnitY();

			THEN("Positive vertex should be the same with centered and unit sphere")
			{
				CHECK(positiveVector == firstCenterAndUnit.GetPositiveVertex(positiveVector));
			}

			AND_THEN("Negative vertex should be the opposite")
			{
				CHECK(-positiveVector == firstCenterAndUnit.GetNegativeVertex(positiveVector));
			}
		}

		WHEN("We extend the unit sphere to one point")
		{
			Nz::Vector3f point = Nz::Vector3f::UnitY() * 2.f;

			firstCenterAndUnit.ExtendTo(point);

			REQUIRE(firstCenterAndUnit.radius == Approx(2.f));

			THEN("Sphere must contain it and distance should be good")
			{
				CHECK(firstCenterAndUnit.Contains(point));
				CHECK(firstCenterAndUnit.Distance(point) == Approx(1.f));
			}
		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Spheref nullRect = Nz::Spheref::Zero();
				Nz::Spheref centerAndUnit = firstCenterAndUnit;
				Nz::Spheref result(Nz::Vector3f::Zero(), 0.5f);

				REQUIRE(Nz::Spheref::Lerp(nullRect, centerAndUnit, 0.5f) == result);
			}
		}
	}
}
