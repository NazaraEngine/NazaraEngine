#include <Nazara/Core/ObjectRef.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Utility/Font.hpp>

SCENARIO("ObjectRef", "[CORE][OBJECTREF]")
{
	GIVEN("A ObjectRef")
	{
		Nz::ObjectRef<Nz::Font> objectRef;

		WHEN("We have two objectRef handling the same object")
		{
			Nz::Font font;

			objectRef = &font;
			Nz::ObjectRef<Nz::Font> otherRef(&font);

			THEN("Pointers the same")
			{
				REQUIRE(objectRef.IsValid());
				REQUIRE(otherRef.IsValid());
			}

			objectRef.Reset(nullptr);
		}

		WHEN("We assign it to a simple font")
		{
			Nz::Font font;

			THEN("Release suppress the reference to the object")
			{
				objectRef.Reset(&font);
				objectRef.Release();

				REQUIRE(!objectRef.IsValid());
			}
		}
	}
}
