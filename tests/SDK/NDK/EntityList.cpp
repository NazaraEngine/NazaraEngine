#include <NDK/EntityList.hpp>
#include <NDK/World.hpp>
#include <Catch/catch.hpp>

SCENARIO("EntityList", "[NDK][ENTITYLIST]")
{
	GIVEN("A world & a set of entities")
	{
		Ndk::World world;
		const Ndk::EntityHandle& entity = world.CreateEntity();
		Ndk::EntityList entityList;
		entityList.Insert(entity);

		WHEN("We ask if entity is in there")
		{
			THEN("These results are expected")
			{
				REQUIRE(entityList.Has(entity->GetId()));
				const Ndk::EntityHandle& newEntity = world.CreateEntity();
				REQUIRE(!entityList.Has(newEntity->GetId()));
			}
		}

		WHEN("We remove then insert")
		{
			entityList.Remove(*entityList.begin());

			THEN("Set should be empty")
			{
				REQUIRE(entityList.empty());
			}

			entityList.Insert(entity);

			THEN("With one element")
			{
				REQUIRE(!entityList.empty());
			}
		}
	}
}