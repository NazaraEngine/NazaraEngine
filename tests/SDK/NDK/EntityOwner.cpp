#include <NDK/EntityOwner.hpp>
#include <NDK/World.hpp>
#include <Catch/catch.hpp>

SCENARIO("EntityOwner", "[NDK][ENTITYOWNER]")
{
	GIVEN("A world & an entity")
	{
		Ndk::World world;
		const Ndk::EntityHandle& entity = world.CreateEntity();

		WHEN("We set the ownership of the entity to our owner")
		{
			Ndk::EntityOwner entityOwner(entity);

			THEN("Entity is still valid")
			{
				REQUIRE(entity.IsValid());
			}

			THEN("Resetting or getting out of scope is no more valid")
			{
				entityOwner.Reset();
				world.Update(1.f);
				REQUIRE(!entity.IsValid());
			}

			THEN("Moving an entity owner works")
			{
				Ndk::EntityOwner entityOwner2(std::move(entityOwner));
				entityOwner2.Reset();
				world.Update(1.f);
				REQUIRE(!entity.IsValid());
			}
		}
	}
}