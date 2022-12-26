#include <Nazara/Core/RefCounted.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

SCENARIO("RefCounted", "[CORE][REFCOUNTED]")
{
	GIVEN("A refcounted persistent")
	{
		Nz::RefCounted refCounted;
		REQUIRE(refCounted.IsPersistent() == true);

		WHEN("We add a reference to this persistent object")
		{
			THEN("Number of references should be one")
			{
				refCounted.AddReference();
				REQUIRE(refCounted.GetReferenceCount() == 1);
				REQUIRE(refCounted.RemoveReference() == false);
			}

			AND_THEN("We suppress the reference, object is still alive")
			{
				refCounted.AddReference();
				REQUIRE(refCounted.IsPersistent());
				REQUIRE(refCounted.RemoveReference() == false);
				REQUIRE(refCounted.GetReferenceCount() == 0);
			}
		}
	}
}
