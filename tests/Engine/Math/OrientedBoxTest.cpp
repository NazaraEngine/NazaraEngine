#include <Nazara/Math/OrientedBox.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("OrientedBox", "[MATH][ORIENTEDBOX]")
{
	GIVEN("Two center and unit oriented boxes")
	{
		Nz::OrientedBoxf firstCenterAndUnit(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
		Nz::OrientedBoxf secondCenterAndUnit(Nz::OrientedBox<int>(Nz::Vector3i::Zero(), Nz::Vector3i::Unit()));

		firstCenterAndUnit.Update(Nz::Matrix4f::Identity());
		secondCenterAndUnit.Update(Nz::Matrix4f::Identity());

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
				firstCenterAndUnit.Update(Nz::Matrix4f::Identity());
				secondCenterAndUnit.Update(Nz::Matrix4f::Scale(Nz::Vector3f::Unit() * 2.f));

				REQUIRE(firstCenterAndUnit != secondCenterAndUnit);
				for (unsigned int i = 0; i < Nz::BoxCornerCount; ++i)
				{
					REQUIRE(firstCenterAndUnit(i) == secondCenterAndUnit(i));
				}
			}
		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::OrientedBoxf nullOrientedBox = Nz::OrientedBoxf::Zero();
				Nz::OrientedBoxf centerAndUnit = firstCenterAndUnit;
				nullOrientedBox.Update(Nz::Matrix4f::Identity());
				centerAndUnit.Update(Nz::Matrix4f::Identity());
				Nz::OrientedBoxf result(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 0.5f);
				result.Update(Nz::Matrix4f::Identity());

				REQUIRE(Nz::OrientedBoxf::Lerp(nullOrientedBox, centerAndUnit, 0.5f) == result);
			}
		}
	}
}
