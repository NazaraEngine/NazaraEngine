#include <Nazara/Core/PrimitiveList.hpp>
#include <Catch/catch.hpp>

SCENARIO("PrimitiveList", "[CORE][PRIMITIVELIST]")
{
	GIVEN("An empty PrimitiveList")
	{
		Nz::PrimitiveList primitiveList;

		WHEN("We add two primitives")
		{
			float size = 1.f;
			unsigned int subdivision = 1;
			unsigned int recursionLevel = 1;
			Nz::Matrix4f identity = Nz::Matrix4f::Identity();

			primitiveList.AddCubicSphere(size, subdivision, identity);
			primitiveList.AddIcoSphere(size, subdivision, identity);

			THEN("There must be two items")
			{
				REQUIRE(primitiveList.GetSize() == 2);
			}

			THEN("The first one is the cubic sphere")
			{
				REQUIRE(primitiveList(0).sphere.type == Nz::SphereType_Cubic);
			}
		}
	}
}
