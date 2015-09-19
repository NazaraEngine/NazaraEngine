#include <Nazara/Math/Box.hpp>
#include <Catch/catch.hpp>

SCENARIO("Box", "[MATH][BOX]")
{
	GIVEN("Two zero boxes")
	{
		NzBoxf firstZero(NzBoxf::Zero());
		NzBoxf secondZero(NzVector3f::Zero(), NzVector3f::Zero());

		WHEN("We multiply them")
		{
			firstZero = firstZero * 1.f;
			secondZero = secondZero * NzVector3f::Unit() * 3.f;

			THEN("They should stay the same")
			{
				REQUIRE(firstZero == secondZero);
				CHECK(!firstZero.IsValid());
				CHECK(!secondZero.IsValid());
			}
		}
	}

	GIVEN("Two unit and center boxes")
	{
		NzBoxf firstCenterAndUnit(NzRectf(NzVector2f::Zero(), NzVector2f::Unit()));
		NzBoxf secondCenterAndUnit(1.f, 1.f, 1.f);

		WHEN("We ask for some informations")
		{
			THEN("These results are expected")
			{
				REQUIRE(firstCenterAndUnit.GetBoundingSphere() == NzSpheref(NzVector3f::Unit() * 0.5f, std::sqrt(3.f * 0.5f * 0.5f)));
				REQUIRE(firstCenterAndUnit.GetCenter() == (NzVector3f::Unit() * 0.5f));
				REQUIRE(firstCenterAndUnit.GetCorner(nzBoxCorner_FarLeftTop) == NzVector3f::UnitY());
				REQUIRE(firstCenterAndUnit.GetLengths() == NzVector3f::Unit());
				REQUIRE(firstCenterAndUnit.GetMaximum() == NzVector3f::Unit());
				REQUIRE(firstCenterAndUnit.GetMinimum() == NzVector3f::Zero());
				REQUIRE(firstCenterAndUnit.GetNegativeVertex(NzVector3f::Unit()) == NzVector3f::Zero());
				REQUIRE(firstCenterAndUnit.GetPosition() == NzVector3f::Zero());
				REQUIRE(firstCenterAndUnit.GetPositiveVertex(NzVector3f::Unit()) == NzVector3f::Unit());
				REQUIRE(firstCenterAndUnit.GetRadius() == Approx(std::sqrt(3.f * 0.5f * 0.5f)));
				REQUIRE(firstCenterAndUnit.GetSquaredBoundingSphere() == NzSpheref(NzVector3f::Unit() * 0.5f, 3.f * 0.5f * 0.5f));
				REQUIRE(firstCenterAndUnit.GetSquaredRadius() == Approx(3.f * 0.5f * 0.5f));
			}
		}

		WHEN("We ask for the intersection between the two")
		{
			THEN("We should have a center and unit")
			{
				NzBoxf thirdCenterAndUnit;
				CHECK(firstCenterAndUnit.Intersect(secondCenterAndUnit, &thirdCenterAndUnit));
				REQUIRE(firstCenterAndUnit == secondCenterAndUnit);
			}
		}

		WHEN("We use the constructor of conversion")
		{
			THEN("Shouldn't be a problem")
			{
				NzBoxf tmp(NzBoxi(0, 0, 0, 1, 1, 1));
				REQUIRE(tmp == firstCenterAndUnit);
			}
		}
	}

	GIVEN("Two wrong box (negative width, height and depth")
	{
		NzBoxf firstWrongBox(-NzVector3f::Unit());
		NzBoxf secondWrongBox(-NzVector3f::Unit());

		WHEN("We check if valid")
		{
			THEN("Result if false")
			{
				CHECK(!firstWrongBox.IsValid());
				CHECK(!secondWrongBox.IsValid());
			}
		}

		WHEN("We correct them")
		{
			firstWrongBox.ExtendTo(NzVector3f::Unit());
			secondWrongBox.Transform(NzMatrix4f::Scale(-NzVector3f::Unit()));

			THEN("They should be valid")
			{
				CHECK(firstWrongBox.IsValid());
				CHECK(secondWrongBox.IsValid());
			}

			AND_WHEN("We ask if they contain boxes")
			{
				THEN("These results are expected")
				{
					CHECK(firstWrongBox.Contains(0.f, 0.f, 0.f));
					CHECK(secondWrongBox.Contains(0.f, 0.f, 0.f));

					secondWrongBox = secondWrongBox.Lerp(NzBoxf::Zero(), secondWrongBox, 0.f); // Zeroed
					secondWrongBox.ExtendTo(NzBoxf(NzVector3f(0.1f, 0.1f, 0.1f), NzVector3f(0.9f, 0.9f, 0.9f)));
					secondWrongBox.Translate(NzVector3f(0.05f, 0.05f, 0.05f)); // Box 0.15 to 0.95
					CHECK(firstWrongBox.Contains(secondWrongBox));

					NzBoxf test(1.f, -500.f, -500.f, 1000.f, 1000.f, 1000.f);
					CHECK(test.Contains(NzBoxf(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f)));
					CHECK(test.Contains(500.f, 0.f, 0.f));
				}
			}
		}
	}
}
