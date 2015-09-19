#include <Nazara/Math/OrientedBox.hpp>
#include <catch.hpp>

SCENARIO("OrientedBox", "[MATH][ORIENTEDBOX]")
{
	GIVEN("Two center and unit oriented boxes")
	{
		NzOrientedBoxf firstCenterAndUnit(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
		NzOrientedBoxf secondCenterAndUnit(NzOrientedBox<int>(NzVector3i::Zero(), NzVector3i::Unit()));

		firstCenterAndUnit.Update(NzMatrix4f::Identity());
		secondCenterAndUnit.Update(NzMatrix4f::Identity());

		WHEN("We compare them")
		{
			THEN("They are the same")
			{
				REQUIRE(firstCenterAndUnit == secondCenterAndUnit);
			}
		}

		WHEN("We ask if they are valid")
		{
			THEN("They are valid")
			{
				CHECK(firstCenterAndUnit.IsValid());
				CHECK(secondCenterAndUnit.IsValid());
			}
		}

		WHEN("We multiply them")
		{
			THEN("Results are different between operator * and update(ScaleMatrix) but corners are the same")
			{
				firstCenterAndUnit *= 2.f;
				firstCenterAndUnit.Update(NzMatrix4f::Identity());
				secondCenterAndUnit.Update(NzMatrix4f::Scale(NzVector3f::Unit() * 2.f));

				REQUIRE(firstCenterAndUnit != secondCenterAndUnit);
				for (unsigned int i = 0; i <= nzBoxCorner_Max; ++i)
				{
					REQUIRE(firstCenterAndUnit(i) == secondCenterAndUnit(i));
				}
			}
		}
	}
}
