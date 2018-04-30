#include <NDK/EntityOwner.hpp>
#include <NDK/World.hpp>
#include <Catch/catch.hpp>

SCENARIO("EntityOwner", "[NDK][ENTITYOWNER]")
{
	GIVEN("A world & an entity")
	{
		Ndk::World world(false);
		Ndk::EntityHandle entity = world.CreateEntity();

		WHEN("We set the ownership of the entity to our owner")
		{
			THEN("Entity is still valid")
			{
				REQUIRE(entity.IsValid());

				Ndk::EntityOwner entityOwner(entity);

				world.Refresh();
				CHECK(entity.IsValid());
			}

			THEN("Moving an entity owner by constructor works")
			{
				REQUIRE(entity.IsValid());

				Ndk::EntityOwner entityOwner(entity);
				Ndk::EntityOwner entityOwner2(std::move(entityOwner));
				entityOwner.Reset();

				world.Refresh();
				CHECK(entity.IsValid());

				entityOwner2.Reset();

				world.Refresh();
				CHECK(!entity.IsValid());
			}

			THEN("Moving an entity owner by operator= works")
			{
				REQUIRE(entity.IsValid());

				Ndk::EntityOwner entityOwner(entity);
				Ndk::EntityOwner entityOwner2;
				entityOwner2 = std::move(entityOwner);
				entityOwner.Reset();

				world.Refresh();
				CHECK(entity.IsValid());

				entityOwner2.Reset();

				world.Refresh();
				CHECK(!entity.IsValid());
			}

			THEN("Destroying an entity owner destroys its entity")
			{
				REQUIRE(entity.IsValid());

				{
					Ndk::EntityOwner entityOwner(entity);
				}

				world.Refresh();
				CHECK(!entity.IsValid());
			}


			THEN("Resetting an entity owner destroys its entity")
			{
				REQUIRE(entity.IsValid());

				Ndk::EntityOwner entityOwner(entity);
				entityOwner.Reset();

				world.Refresh();
				CHECK(!entity.IsValid());
			}

			THEN("Assigning another entity destroys the first entity")
			{
				REQUIRE(entity.IsValid());

				Ndk::EntityOwner entityOwner(entity);
				entityOwner = world.CreateEntity();

				world.Refresh();
				CHECK(!entity.IsValid());
			}

			THEN("Moving another entity destroys the first entity")
			{
				REQUIRE(entity.IsValid());

				Ndk::EntityOwner entityOwner(entity);
				Ndk::EntityHandle entity2 = world.CreateEntity();

				entityOwner = std::move(entity2);

				world.Refresh();
				CHECK(!entity.IsValid());
			}
		}
	}
}
