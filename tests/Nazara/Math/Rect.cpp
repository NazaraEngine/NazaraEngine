#include <Nazara/Math/Rect.hpp>
#include <catch.hpp>

SCENARIO("Rect", "[MATH][RECT]")
{
	GIVEN("Two same rectangles center and unit lengths")
	{
		NzRectf firstCenterAndUnit(0.f, 0.f, 1.f, 1.f);
		NzRectf secondCenterAndUnit(NzRecti(NzVector2i::Unit(), NzVector2i::Zero()));

		WHEN("We ask if they are the same")
		{
			THEN("They should be")
			{
				REQUIRE(firstCenterAndUnit == secondCenterAndUnit);
				REQUIRE(firstCenterAndUnit.GetCenter() == secondCenterAndUnit.GetCenter());
				REQUIRE(firstCenterAndUnit.GetCorner(nzRectCorner_LeftBottom) == secondCenterAndUnit.GetCorner(nzRectCorner_LeftBottom));
				CHECK(firstCenterAndUnit.IsValid());
			}
		}

		WHEN("We move one from (0.5, 0.5)")
		{
			firstCenterAndUnit.Translate(NzVector2f(0.5f, 0.5f));

			THEN("The collision should be (0.5, 0.5) -> (0.5, 0.5)")
			{
				NzRectf tmp;
				CHECK(firstCenterAndUnit.Intersect(secondCenterAndUnit, &tmp));
				REQUIRE(tmp == NzRectf(0.5f, 0.5f, 0.5f, 0.5f));
			}
		}

		WHEN("We make an empty")
		{
			THEN("It's not valid")
			{
				CHECK(!(firstCenterAndUnit * 0.f).IsValid());
			}
		}

		WHEN("We ask for infos")
		{
			THEN("These results are expected")
			{
				REQUIRE(firstCenterAndUnit.GetLengths() == NzVector2f::Unit());
				REQUIRE(firstCenterAndUnit.GetMaximum() == NzVector2f::Unit());
				REQUIRE(firstCenterAndUnit.GetMinimum() == NzVector2f::Zero());
				REQUIRE(firstCenterAndUnit.GetNegativeVertex(NzVector2f::Unit()) == NzVector2f::Zero());
				REQUIRE(firstCenterAndUnit.GetPosition() == NzVector2f::Zero());
				REQUIRE(firstCenterAndUnit.GetPositiveVertex(NzVector2f::Unit()) == NzVector2f::Unit());

			}
		}
	}
}
