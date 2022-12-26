#include <Nazara/Math/Ray.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Ray", "[MATH][RAY]")
{
	GIVEN("Two same Rays (0, 0, 0) -> (0, 1, 0)")
	{
		Nz::Rayf ray(Nz::Ray<int>(Nz::Plane<int>::XY(), Nz::Plane<int>::YZ()));
		Nz::Rayf secondRay(0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

		WHEN("We compare them")
		{
			THEN("They are the same and Y axis")
			{
				REQUIRE(ray == secondRay);
				REQUIRE(ray == Nz::Rayf::AxisY());
			}
		}

		WHEN("We ask for the closest point")
		{
			THEN("The point that is multiple on the Nz::Ray, is at multiple")
			{
				REQUIRE(ray.ClosestPoint(secondRay.GetPoint(1.f)) == Catch::Approx(1.f));
			}
		}

		WHEN("We ask for intersection")
		{
			THEN("For the Box collision's")
			{
				float tmpClosest;
				float tmpFurthest;

				REQUIRE(ray.Intersect(Nz::Boxf(-0.5f, 1.f, -0.5f, 1.f, 1.f, 1.f), &tmpClosest, &tmpFurthest));
				CHECK(ray.GetPoint(tmpClosest) == Nz::Vector3f::UnitY());
				CHECK(ray.GetPoint(tmpFurthest) == (Nz::Vector3f::UnitY() * 2.f));
				REQUIRE(!ray.Intersect(Nz::Boxf(-10.f, 1.f, -10.f, 1.f, 1.f, 1.f), &tmpClosest, &tmpFurthest));
			}

			THEN("For the Nz::Plane collision's")
			{
				float tmpHit = -1.f;

				REQUIRE(ray.Intersect(Nz::Planef(Nz::Vector3f::UnitY(), 1.f), &tmpHit));
				CHECK(ray.GetPoint(tmpHit) == Nz::Vector3f::UnitY());
				REQUIRE(ray.Intersect(Nz::Planef::XZ(), &tmpHit));
				CHECK(ray.GetPoint(tmpHit) == Nz::Vector3f::Zero());
				REQUIRE(ray.Intersect(Nz::Planef(Nz::Vector3f::UnitY(), 2.f), &tmpHit));
				CHECK(ray.GetPoint(tmpHit) == 2.f * Nz::Vector3f::UnitY());

				CHECK(!ray.Intersect(Nz::Planef(Nz::Vector3f::UnitX(), 1.f)));
			}

			THEN("For the Sphere collision's")
			{
				float tmpClosest;
				float tmpFurthest;

				CHECK(ray.Intersect(Nz::Spheref(Nz::Vector3f::UnitY(), 0.1f), &tmpClosest, &tmpFurthest));
				REQUIRE(ray.GetPoint(tmpClosest) == Nz::Vector3f::UnitY() * 0.9f);
				REQUIRE(ray.GetPoint(tmpFurthest) == (Nz::Vector3f::UnitY() * 1.1f));

				CHECK(!ray.Intersect(Nz::Spheref(Nz::Vector3f::UnitX(), 0.9f)));
			}

			THEN("For the bounding volume collision's")
			{
				Nz::BoundingVolumef nullVolume(Nz::Extend::Null);
				CHECK(!ray.Intersect(nullVolume));

				float tmpClosest = -1.f;
				float tmpFurthest = -1.f;
				Nz::BoundingVolumef infiniteVolume(Nz::Extend::Infinite);
				CHECK(ray.Intersect(infiniteVolume, &tmpClosest, &tmpFurthest));
				CHECK(tmpClosest == Catch::Approx(0.f));
				CHECK(tmpFurthest == std::numeric_limits<float>::infinity());
			}

			THEN("For the triangle collision's")
			{
				Nz::Vector3f firstPoint(0.f, 1.f, 1.f);
				Nz::Vector3f secondPoint(-1.f, 1.f, -1.f);
				Nz::Vector3f thidPoint(1.f, 1.f, -1.f);
				float tmpHit = -1.f;

				CHECK(ray.Intersect(firstPoint, secondPoint, thidPoint, &tmpHit));
				REQUIRE(ray.GetPoint(tmpHit) == Nz::Vector3f::UnitY());

				Nz::Vector3f offset = Nz::Vector3f(10.f, 0.f, 10.f);
				CHECK(!ray.Intersect(firstPoint + offset, secondPoint + offset, thidPoint + offset, &tmpHit));
			}
		}

		WHEN("We try to lerp")
		{
			THEN("Compilation should be fine")
			{
				Nz::Rayf AxisX = Nz::Rayf::AxisX();
				Nz::Rayf AxisY = Nz::Rayf::AxisY();
				REQUIRE(Nz::Rayf::Lerp(AxisX, AxisY, 0.5f) == (Nz::Rayf(Nz::Vector3f::Zero(), Nz::Vector3f(0.5f, 0.5f, 0.f))));
			}
		}
	}
}
