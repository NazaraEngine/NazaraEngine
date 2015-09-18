#include <Nazara/Math/BoundingVolume.hpp>
#include <Catch/catch.hpp>

SCENARIO("BoundingVolume", "[MATH][BOUNDINGVOLUME]")
{
	GIVEN("With a null bounding volume and an infinite")
	{
		NzBoundingVolumef nullVolume = NzBoundingVolumef::Null();
		NzBoundingVolumef infiniteVolume = NzBoundingVolumef::Infinite();

		WHEN("We compare them")
		{
			THEN("They should be different")
			{
				REQUIRE(nullVolume != infiniteVolume);
			}
		}

		WHEN("We ask for the characteristic")
		{
			THEN("They should be respectively null and infinite")
			{
				CHECK(nullVolume.IsNull());
				CHECK(infiniteVolume.IsInfinite());
			}
		}

		WHEN("If we multiply them")
		{
			THEN("They should still be different")
			{
				nullVolume *= 5.f;
				infiniteVolume = infiniteVolume * 0.5f;

				REQUIRE(nullVolume != infiniteVolume);

				AND_WHEN("We ask for the characteristic (infinite and null)")
				{
					THEN("They should still be respectively null and infinite")
					{
						CHECK(nullVolume.IsNull());
						CHECK(infiniteVolume.IsInfinite());
					}
				}
			}
		}

		WHEN("We compare two null or two infinite")
		{
			THEN("Everything should be ok")
			{
				REQUIRE(NzBoundingVolumef::Null() == NzBoundingVolumef::Null());
				REQUIRE(NzBoundingVolumef::Infinite() == NzBoundingVolumef::Infinite());
			}
		}
	}

	GIVEN("Two same bounding volume with different constructor")
	{
		NzBoundingVolumef firstCenterAndUnit(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
		NzBoundingVolumef secondCenterAndUnit(NzVector3f::Zero(), NzVector3f::Unit());
		firstCenterAndUnit.Update(NzMatrix4f::Identity());
		secondCenterAndUnit.Update(NzMatrix4f::Identity());

		WHEN("We compare them")
		{
			THEN("Then the should be equal")
			{
				REQUIRE(firstCenterAndUnit == secondCenterAndUnit);
			}
		}

		WHEN("We ask for the characteristic")
		{
			THEN("They should be finite")
			{
				CHECK(firstCenterAndUnit.IsFinite());
				CHECK(secondCenterAndUnit.IsFinite());
			}
		}

		WHEN("We use a constructor of conversion")
		{
			THEN("There's no problem")
			{
				NzBoundingVolume<int> intVolumeCenterAndUnit(NzBoxi(NzVector3i::Zero(), NzVector3i::Unit()));
				NzBoundingVolumef thirdCenterAndUnit(intVolumeCenterAndUnit);
				REQUIRE(thirdCenterAndUnit == secondCenterAndUnit);
			}
		}

		WHEN("We make one twice bigger with a matrix")
		{
			firstCenterAndUnit.Update(NzMatrix4f::Scale(NzVector3f::Unit() * 2.f));

			THEN("The local box should be the same but the aabb different")
			{
				REQUIRE(firstCenterAndUnit.obb.localBox == secondCenterAndUnit.obb.localBox);
				REQUIRE(firstCenterAndUnit.aabb != secondCenterAndUnit.aabb);
			}
		}
	}
}
