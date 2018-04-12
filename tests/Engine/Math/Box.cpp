#include <Nazara/Math/Box.hpp>
#include <Catch/catch.hpp>

SCENARIO("Box", "[MATH][BOX]")
{
	GIVEN("Two zero boxes")
	{
		Nz::Boxf firstZero(Nz::Boxf::Zero());
		Nz::Boxf secondZero(Nz::Vector3f::Zero(), Nz::Vector3f::Zero());

		WHEN("We multiply them")
		{
			firstZero = firstZero * 1.f;
			secondZero = secondZero * Nz::Vector3f::Unit() * 3.f;

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
		Nz::Boxf firstCenterAndUnit(Nz::Rectf(Nz::Vector2f::Zero(), Nz::Vector2f::Unit()));
		Nz::Boxf secondCenterAndUnit(1.f, 1.f, 1.f);

		WHEN("We ask for some informations")
		{
			THEN("These results are expected")
			{
				REQUIRE(firstCenterAndUnit.GetBoundingSphere() == Nz::Spheref(Nz::Vector3f::Unit() * 0.5f, std::sqrt(3.f * 0.5f * 0.5f)));
				REQUIRE(firstCenterAndUnit.GetCenter() == (Nz::Vector3f::Unit() * 0.5f));
				REQUIRE(firstCenterAndUnit.GetCorner(Nz::BoxCorner_FarLeftTop) == Nz::Vector3f::UnitY());
				REQUIRE(firstCenterAndUnit.GetLengths() == Nz::Vector3f::Unit());
				REQUIRE(firstCenterAndUnit.GetMaximum() == Nz::Vector3f::Unit());
				REQUIRE(firstCenterAndUnit.GetMinimum() == Nz::Vector3f::Zero());
				REQUIRE(firstCenterAndUnit.GetNegativeVertex(Nz::Vector3f::Unit()) == Nz::Vector3f::Zero());
				REQUIRE(firstCenterAndUnit.GetPosition() == Nz::Vector3f::Zero());
				REQUIRE(firstCenterAndUnit.GetPositiveVertex(Nz::Vector3f::Unit()) == Nz::Vector3f::Unit());
				REQUIRE(firstCenterAndUnit.GetRadius() == Approx(std::sqrt(3.f * 0.5f * 0.5f)));
				REQUIRE(firstCenterAndUnit.GetSquaredBoundingSphere() == Nz::Spheref(Nz::Vector3f::Unit() * 0.5f, 3.f * 0.5f * 0.5f));
				REQUIRE(firstCenterAndUnit.GetSquaredRadius() == Approx(3.f * 0.5f * 0.5f));
			}
		}

		WHEN("We ask for the intersection between the two")
		{
			THEN("We should have a center and unit")
			{
				Nz::Boxf thirdCenterAndUnit;
				CHECK(firstCenterAndUnit.Intersect(secondCenterAndUnit, &thirdCenterAndUnit));
				REQUIRE(firstCenterAndUnit == secondCenterAndUnit);
			}
		}

		WHEN("We ask for the intersection when there are none")
		{
			firstCenterAndUnit.Translate(Nz::Vector3f::UnitZ() * 5.f);
			THEN("We should have a center and unit")
			{
				Nz::Boxf thirdCenterAndUnit;
				CHECK(!firstCenterAndUnit.Intersect(secondCenterAndUnit, &thirdCenterAndUnit));
			}
		}

		WHEN("We use the constructor of conversion")
		{
			THEN("Shouldn't be a problem")
			{
				Nz::Boxf tmp(Nz::Boxi(0, 0, 0, 1, 1, 1));
				REQUIRE(tmp == firstCenterAndUnit);
			}
		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Boxf nullBox = Nz::Boxf::Zero();
				Nz::Boxf centerAndUnit = firstCenterAndUnit;
				Nz::Boxf result(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 0.5f);

				REQUIRE(Nz::Boxf::Lerp(nullBox, centerAndUnit, 0.5f) == result);
			}
		}
	}

	GIVEN("Two wrong box (negative width, height and depth")
	{
		Nz::Boxf firstWrongBox(-Nz::Vector3f::Unit());
		Nz::Boxf secondWrongBox(-Nz::Vector3f::Unit());

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
			firstWrongBox.ExtendTo(Nz::Vector3f::Unit());
			secondWrongBox.Transform(Nz::Matrix4f::Scale(-Nz::Vector3f::Unit()));

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

					secondWrongBox = secondWrongBox.Lerp(Nz::Boxf::Zero(), secondWrongBox, 0.f); // Zeroed
					secondWrongBox.ExtendTo(Nz::Boxf(Nz::Vector3f(0.1f, 0.1f, 0.1f), Nz::Vector3f(0.9f, 0.9f, 0.9f)));
					secondWrongBox.Translate(Nz::Vector3f(0.05f, 0.05f, 0.05f)); // Box 0.15 to 0.95
					CHECK(firstWrongBox.Contains(secondWrongBox));

					Nz::Boxf test(1.f, -500.f, -500.f, 1000.f, 1000.f, 1000.f);
					CHECK(test.Contains(Nz::Boxf(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f)));
					CHECK(test.Contains(500.f, 0.f, 0.f));
				}
			}
		}
	}
}
