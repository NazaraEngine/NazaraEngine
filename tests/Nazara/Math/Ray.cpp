#include <Nazara/Math/Ray.hpp>
#include <catch.hpp>

SCENARIO("Ray", "[RAY]")
{
	GIVEN("Two same rays (0, 0, 0) -> (0, 1, 0)")
	{
		NzRayf firstRay(NzRay<int>(NzPlane<int>::XY(), NzPlane<int>::YZ()));
		NzRayf secondRay(0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

		WHEN("We compare them")
		{
			THEN("They are the same and Y axis")
			{
				REQUIRE(firstRay == secondRay);
				REQUIRE(firstRay == NzRayf::AxisY());
			}
		}

		WHEN("We ask for the closest point")
		{
			THEN("The point that is multiple on the ray, is at multiple")
			{
				REQUIRE(firstRay.ClosestPoint(secondRay.GetPoint(1.f)) == Approx(1.f));
			}
		}

		WHEN("We ask for intersection")
		{
			THEN("For the Box collision's")
			{
				float tmpClosest;
				float tmpFurthest;

				CHECK(firstRay.Intersect(NzBoxf(-0.5f, 1.f, -0.5f, 1.f, 1.f, 1.f), &tmpClosest, &tmpFurthest));
				REQUIRE(firstRay.GetPoint(tmpClosest) == NzVector3f::UnitY());
				REQUIRE(firstRay.GetPoint(tmpFurthest) == (NzVector3f::UnitY() * 2.f));
				CHECK(!firstRay.Intersect(NzBoxf(-10.f, 1.f, -10.f, 1.f, 1.f, 1.f), &tmpClosest, &tmpFurthest));
			}

			THEN("For the Plane collision's")
			{
				float tmpHit;

				CHECK(firstRay.Intersect(NzPlanef(NzVector3f::UnitY(), 1.f), &tmpHit));
				REQUIRE(firstRay.GetPoint(tmpHit) == NzVector3f::UnitY());
				CHECK(firstRay.Intersect(NzPlanef::XZ(), &tmpHit));
				REQUIRE(firstRay.GetPoint(tmpHit) == NzVector3f::Zero());
				CHECK(firstRay.Intersect(NzPlanef(NzVector3f::UnitY(), 2.f), &tmpHit));
				REQUIRE(firstRay.GetPoint(tmpHit) == 2.f * NzVector3f::UnitY());

				CHECK(!firstRay.Intersect(NzPlanef(NzVector3f::UnitX(), 1.f)));
			}

			THEN("For the Sphere collision's")
			{
				float tmpClosest;
				float tmpFurthest;

				CHECK(firstRay.Intersect(NzSpheref(NzVector3f::UnitY(), 0.1f), &tmpClosest, &tmpFurthest));
				REQUIRE(firstRay.GetPoint(tmpClosest) == NzVector3f::UnitY() * 0.9f);
				REQUIRE(firstRay.GetPoint(tmpFurthest) == (NzVector3f::UnitY() * 1.1f));

				CHECK(!firstRay.Intersect(NzSpheref(NzVector3f::UnitX(), 0.9f)));
			}

			THEN("For the OBB collision's")
			{
				float tmpClosest;
				float tmpFurthest;

				NzOrientedBoxf obb(-0.5f, 1.f, -0.5f, 1.f, 1.f, 1.f);
				obb.Update(NzMatrix4f::Rotate(NzEulerAnglesf(0.f, 90.f, 0.f).ToQuaternion()));

				CHECK(firstRay.Intersect(obb, &tmpClosest, &tmpFurthest));
				REQUIRE(firstRay.GetPoint(tmpClosest) == NzVector3f::UnitY());
				REQUIRE(firstRay.GetPoint(tmpFurthest) == (NzVector3f::UnitY() * 2.f));

				obb = NzOrientedBoxf(-10.f, 1.f, -10.f, 1.f, 1.f, 1.f);
				obb.Update(NzMatrix4f::Rotate(NzEulerAnglesf(0.f, 0.f, 90.f).ToQuaternion()));
				CHECK(!firstRay.Intersect(obb, &tmpClosest, &tmpFurthest));
			}

			THEN("For the bounding volume collision's")
			{
				NzBoundingVolumef nullVolume(nzExtend_Null);
				CHECK(!firstRay.Intersect(nullVolume));

				NzBoundingVolumef infiniteVolume(nzExtend_Infinite);
				CHECK(firstRay.Intersect(infiniteVolume));
			}

		}
	}
}
