#include <NazaraSDK/EntityOwner.hpp>
#include <NazaraSDK/World.hpp>
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

	GIVEN("A vector of EntityOwner")
	{
		Ndk::World world(false);

		std::vector<Ndk::EntityOwner> entityOwners;
		for (std::size_t i = 1; i <= 10; ++i)
		{
			for (const Ndk::EntityHandle& entity : world.CreateEntities(10 * i))
				entityOwners.emplace_back(entity);

			entityOwners.clear();
			world.Refresh();

			std::size_t aliveEntities = 0;
			for (const Ndk::EntityHandle& entity : world.GetEntities())
				aliveEntities++;

			CHECK(aliveEntities == 0);
		}
	}
}
