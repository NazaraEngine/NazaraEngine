#include <Nazara/Math/Frustum.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Frustum", "[MATH][FRUSTUM]")
{
	GIVEN("One frustum (90, 1, 1, 1000, (0, 0, 0), (1, 0, 0))")
	{
		Nz::Frustumf frustum = Nz::Frustumf::Build(Nz::DegreeAnglef(90.f), 1.f, 1.f, 1000.f, Nz::Vector3f::Zero(), Nz::Vector3f::UnitX());

		WHEN("We ask for intersection with objects outside the frustum")
		{
			THEN("These results are expected")
			{
				Nz::BoundingVolumef bv(Nz::Vector3f::Zero(), Nz::Vector3f::Unit());
				bv.Update(Nz::Matrix4f::Identity());
				REQUIRE(Nz::IntersectionSide::Outside == frustum.Intersect(bv));
				REQUIRE(Nz::IntersectionSide::Outside == frustum.Intersect(Nz::Boxf(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 0.9f)));
				Nz::OrientedBoxf obb(Nz::Vector3f::Zero(), Nz::Vector3f::Unit() * 0.9f);
				obb.Update(Nz::Matrix4f::Identity());
				REQUIRE(Nz::IntersectionSide::Outside == frustum.Intersect(obb));
				REQUIRE(Nz::IntersectionSide::Outside == frustum.Intersect(Nz::Spheref(Nz::Vector3f::Zero(), 0.5f)));
				Nz::Vector3f tmp = Nz::Vector3f::Zero();
				REQUIRE(Nz::IntersectionSide::Outside == frustum.Intersect(&tmp, 1));
				tmp = Nz::Vector3f::UnitX() * -10.f;
				REQUIRE(Nz::IntersectionSide::Outside == frustum.Intersect(&tmp, 1));
			}
		}

		WHEN("We ask for intersection with objects inside the frustum")
		{
			THEN("These results are expected")
			{
				Nz::BoundingVolumef bv(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f);
				bv.Update(Nz::Matrix4f::Identity());

				REQUIRE(Nz::IntersectionSide::Inside == frustum.Intersect(bv));
				REQUIRE(Nz::IntersectionSide::Inside == frustum.Intersect(Nz::Boxf(Nz::Vector3f::UnitX() * 500.f, Nz::Vector3f::Unit())));
				Nz::OrientedBoxf obb(Nz::Vector3f::UnitX() * 100.f, Nz::Vector3f::Unit());
				obb.Update(Nz::Matrix4f::Identity());
				REQUIRE(Nz::IntersectionSide::Inside == frustum.Intersect(obb));
				REQUIRE(Nz::IntersectionSide::Inside == frustum.Intersect(Nz::Spheref(Nz::Vector3f::UnitX() * 100.f, 0.5f)));
				Nz::Vector3f tmp = Nz::Vector3f::UnitX() * 100.f;
				REQUIRE(Nz::IntersectionSide::Inside == frustum.Intersect(&tmp, 1));
			}
		}

		WHEN("We ask for contains with objects outside the frustum")
		{
			THEN("These results are expected")
			{
				Nz::BoundingVolumef bv(0.f, -0.25f, -0.25f, 0.5f, 0.5f, 0.5f);
				bv.Update(Nz::Matrix4f::Identity());
				CHECK(!frustum.Contains(bv));
				CHECK(!frustum.Contains(Nz::Boxf(0.f, -0.25f, -0.25f, 0.5f, 0.5f, 0.5f)));
				Nz::OrientedBoxf obb(0.f, -0.25f, -0.25f, 0.5f, 0.5f, 0.5f);
				obb.Update(Nz::Matrix4f::Identity());
				CHECK(!frustum.Contains(obb));
				CHECK(!frustum.Contains(Nz::Spheref(Nz::Vector3f::Zero(), 0.5f)));
				Nz::Vector3f tmp = Nz::Vector3f::Zero();
				CHECK(!frustum.Contains(&tmp, 1));
			}
		}

		WHEN("We ask for contains with objects inside the frustum")
		{
			THEN("These results are expected")
			{
				Nz::BoundingVolumef bv(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f);
				bv.Update(Nz::Matrix4f::Identity());
				CHECK(frustum.Contains(bv));
				CHECK(frustum.Contains(Nz::Boxf(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f)));
				Nz::OrientedBoxf obb(500.f, -0.5f, -0.5f, 1.f, 1.f, 1.f);
				obb.Update(Nz::Matrix4f::Identity());
				CHECK(frustum.Contains(obb));
				CHECK(frustum.Contains(Nz::Spheref(Nz::Vector3f::UnitX() * 500.f, 1.f)));
				Nz::Vector3f tmp = Nz::Vector3f::UnitX() * 500.f;
				CHECK(frustum.Contains(&tmp, 1));
			}
		}

		WHEN("We test for edge cases")
		{
			THEN("Implementation defined these")
			{
				Nz::BoundingVolumef nullVolume = Nz::BoundingVolumef::Null();
				CHECK(!frustum.Contains(nullVolume));
				Nz::BoundingVolumef infiniteVolume = Nz::BoundingVolumef::Infinite();
				CHECK(frustum.Contains(infiniteVolume));
				REQUIRE(frustum.Intersect(nullVolume) == Nz::IntersectionSide::Outside);
				REQUIRE(frustum.Intersect(infiniteVolume) == Nz::IntersectionSide::Intersecting);
			}
		}
	}
}
