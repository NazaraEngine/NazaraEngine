#include <Nazara/Math/Frustum.hpp>
#include <catch.hpp>

SCENARIO("Frustum", "[MATH][FRUSTUM]")
{
	GIVEN("One frustum (90, 1, 1, 1000, (0, 0, 0), (1, 0, 0))")
	{
		NzFrustumf frustum;
		frustum.Build(NzFromDegrees(90.f), 1.f, 1.f, 1000.f, NzVector3f::Zero(), NzVector3f::UnitX());

		WHEN("We ask for intersection with objects outside the frustum")
		{
			THEN("These results are expected")
			{
				NzBoundingVolumef bv(NzVector3f::Zero(), NzVector3f::Unit());
				bv.Update(NzMatrix4f::Identity());
				REQUIRE(nzIntersectionSide_Outside == frustum.Intersect(bv));
				REQUIRE(nzIntersectionSide_Outside == frustum.Intersect(NzBoxf(NzVector3f::Zero(), NzVector3f::Unit() * 0.9f)));
				NzOrientedBoxf obb(NzVector3f::Zero(), NzVector3f::Unit() * 0.9f);
				obb.Update(NzMatrix4f::Identity());
				REQUIRE(nzIntersectionSide_Outside == frustum.Intersect(obb));
				REQUIRE(nzIntersectionSide_Outside == frustum.Intersect(NzSpheref(NzVector3f::Zero(), 0.5f)));
				NzVector3f tmp = NzVector3f::Zero();
				REQUIRE(nzIntersectionSide_Outside == frustum.Intersect(&tmp, 1));
				tmp = NzVector3f::UnitX() * -10.f;
				REQUIRE(nzIntersectionSide_Outside == frustum.Intersect(&tmp, 1));
			}
		}

		WHEN("We ask for intersection with objects inside the frustum")
		{
			THEN("These results are expected")
			{
				NzBoundingVolumef bv(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f);
				bv.Update(NzMatrix4f::Identity());

				REQUIRE(nzIntersectionSide_Inside == frustum.Intersect(bv));
				REQUIRE(nzIntersectionSide_Inside == frustum.Intersect(NzBoxf(NzVector3f::UnitX() * 500.f, NzVector3f::Unit())));
				NzOrientedBoxf obb(NzVector3f::UnitX() * 100.f, NzVector3f::Unit());
				obb.Update(NzMatrix4f::Identity());
				REQUIRE(nzIntersectionSide_Inside == frustum.Intersect(obb));
				REQUIRE(nzIntersectionSide_Inside == frustum.Intersect(NzSpheref(NzVector3f::UnitX() * 100.f, 0.5f)));
				NzVector3f tmp = NzVector3f::UnitX() * 100.f;
				REQUIRE(nzIntersectionSide_Inside == frustum.Intersect(&tmp, 1));
			}
		}

		WHEN("We ask for contains with objects outside the frustum")
		{
			THEN("These results are expected")
			{
				NzBoundingVolumef bv(0.f, -0.25f, -0.25f, 0.5f, 0.5f, 0.5f);
				bv.Update(NzMatrix4f::Identity());
				CHECK(!frustum.Contains(bv));
				CHECK(!frustum.Contains(NzBoxf(0.f, -0.25f, -0.25f, 0.5f, 0.5f, 0.5f)));
				NzOrientedBoxf obb(0.f, -0.25f, -0.25f, 0.5f, 0.5f, 0.5f);
				obb.Update(NzMatrix4f::Identity());
				CHECK(!frustum.Contains(obb));
				CHECK(!frustum.Contains(NzSpheref(NzVector3f::Zero(), 0.5f)));
				NzVector3f tmp = NzVector3f::Zero();
				CHECK(!frustum.Contains(&tmp, 1));
			}
		}

		WHEN("We ask for contains with objects inside the frustum")
		{
			THEN("These results are expected")
			{
				NzBoundingVolumef bv(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f);
				bv.Update(NzMatrix4f::Identity());
				CHECK(frustum.Contains(bv));
				CHECK(frustum.Contains(NzBoxf(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f)));
				NzOrientedBoxf obb(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f);
				obb.Update(NzMatrix4f::Identity());
				CHECK(frustum.Contains(obb));
				CHECK(frustum.Contains(NzSpheref(NzVector3f::UnitX() * 500.f, 1.f)));
				NzVector3f tmp = NzVector3f::UnitX() * 500.f;
				CHECK(frustum.Contains(&tmp, 1));
			}
		}
	}
}
