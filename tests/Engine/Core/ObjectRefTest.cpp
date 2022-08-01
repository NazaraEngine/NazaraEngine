#include <Nazara/Core/ObjectRef.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

class Test : public Nz::RefCounted
{
};

SCENARIO("ObjectRef", "[CORE][OBJECTREF]")
{
	GIVEN("A ObjectRef")
	{
		Nz::ObjectRef<Test> objectRef;

		WHEN("We have two objectRef handling the same object")
		{
			Test test;

			objectRef = &test;
			Nz::ObjectRef<Test> otherRef(&test);

			THEN("Pointers the same")
			{
				REQUIRE(objectRef.IsValid());
				REQUIRE(otherRef.IsValid());
			}

			objectRef.Reset(nullptr);
		}

		WHEN("We assign it to a simple font")
		{
			Test test;

			THEN("Release suppress the reference to the object")
			{
				objectRef.Reset(&test);
				objectRef.Release();

				REQUIRE(!objectRef.IsValid());
			}
		}
	}
}
