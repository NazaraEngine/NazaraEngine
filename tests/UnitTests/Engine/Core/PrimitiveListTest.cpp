#include <Nazara/Core/PrimitiveList.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("PrimitiveList", "[CORE][PRIMITIVELIST]")
{
	GIVEN("An empty PrimitiveList")
	{
		Nz::PrimitiveList primitiveList;

		WHEN("We add two primitives")
		{
			float size = 1.f;
			unsigned int subdivision = 1;
			Nz::Matrix4f identity = Nz::Matrix4f::Identity();

			primitiveList.AddCubicSphere(size, subdivision, identity);
			primitiveList.AddBox(Nz::Vector3f(size), Nz::Vector3ui(subdivision), identity);
			primitiveList.AddIcoSphere(size, subdivision, identity);

			THEN("There must be two items")
			{
				REQUIRE(primitiveList.GetSize() == 3);
			}

			THEN("The first one is the cubic sphere")
			{
				REQUIRE(primitiveList[0].type == Nz::PrimitiveType::Sphere);
				REQUIRE(primitiveList[0].sphere.type == Nz::SphereType::Cubic);
			}

			THEN("The second one is the box")
			{
				REQUIRE(primitiveList[1].type == Nz::PrimitiveType::Box);
			}

			THEN("The third one is the ico sphere")
			{
				REQUIRE(primitiveList[2].type == Nz::PrimitiveType::Sphere);
				REQUIRE(primitiveList[2].sphere.type == Nz::SphereType::Ico);
			}
		}
	}
}
