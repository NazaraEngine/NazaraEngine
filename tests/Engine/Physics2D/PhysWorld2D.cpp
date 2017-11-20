#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <Catch/catch.hpp>

Nz::RigidBody2D CreateBody(Nz::PhysWorld2D& world, const Nz::Vector2f& position, bool isMoving = true, const Nz::Vector2f& lengths = Nz::Vector2f::Unit());

Nz::UInt32 collisionGroup = 1;
Nz::UInt32 categoryMask = 2;
Nz::UInt32 collisionMask = 3;

SCENARIO("PhysWorld2D", "[PHYSICS2D][PHYSWORLD2D]")
{
	GIVEN("A physic world and a bunch of entities on a grid")
	{
		Nz::PhysWorld2D world;

		std::vector<Nz::RigidBody2D> bodies;
		const int numberOfBodiesPerLign = 3;
		for (int i = 0; i != numberOfBodiesPerLign; ++i)
		{
			for (int j = 0; j != numberOfBodiesPerLign; ++j)
			{
				bodies.push_back(CreateBody(world, Nz::Vector2f(10.f * i, 10.f * j)));
			}
		}

		world.Step(1.f);

		WHEN("We ask for the nearest body")
		{
			Nz::PhysWorld2D::NearestQueryResult result;
			REQUIRE(world.NearestBodyQuery(-Nz::Vector2f::UnitY() * 1.f, 2.f, collisionGroup, categoryMask, collisionMask, &result));

			THEN("It should be the one on the origin")
			{
				CHECK(result.nearestBody == &bodies[0]);
				CHECK(result.closestPoint == Nz::Vector2f::Zero());
				CHECK(result.fraction == -Nz::Vector2f::UnitY());
				CHECK(result.distance == Approx(1.f));
			}

			REQUIRE(world.NearestBodyQuery(Nz::Vector2f::UnitY() * 2.f, 2.f, collisionGroup, categoryMask, collisionMask, &result));

			THEN("It should be the one on the origin")
			{
				CHECK(result.nearestBody == &bodies[0]);
				CHECK(result.closestPoint == Nz::Vector2f::UnitY());
				CHECK(result.fraction == Nz::Vector2f::UnitY());
				CHECK(result.distance == Approx(1.f));
			}
		}

		WHEN("We ask for the first ray collision")
		{
			Nz::Vector2f origin = -Nz::Vector2f::UnitY() * 2.f;
			Nz::Vector2f end = (numberOfBodiesPerLign + 1) * 10.f * Nz::Vector2f::UnitY();
			Nz::PhysWorld2D::RaycastHit result;
			REQUIRE(world.RaycastQueryFirst(origin, end, 1.f, collisionGroup, categoryMask, collisionMask, &result));

			THEN("It should be the one on the origin")
			{
				CHECK(result.nearestBody == &bodies[0]);
				CHECK(result.fraction == Approx(1.f / 42.f));
				CHECK(result.hitPos == Nz::Vector2f::Zero());
				CHECK(result.hitNormal == -Nz::Vector2f::UnitY());
			}
		}

		WHEN("We ask for the ray collisions")
		{
			Nz::Vector2f origin = -Nz::Vector2f::UnitY() * 2.f;
			Nz::Vector2f end = (numberOfBodiesPerLign + 1) * 10.f * Nz::Vector2f::UnitY();
			std::vector<Nz::PhysWorld2D::RaycastHit> results;
			REQUIRE(world.RaycastQuery(origin, end, 1.f, collisionGroup, categoryMask, collisionMask, &results));

			THEN("It should be the first lign")
			{
				REQUIRE(results.size() == numberOfBodiesPerLign);

				for (int i = 0; i != numberOfBodiesPerLign; ++i)
				{
					const Nz::PhysWorld2D::RaycastHit& result = results[i];
					CHECK(result.nearestBody == &bodies[i]);
					CHECK(result.fraction == Approx(i / 4.f).epsilon(0.1f));
					CHECK(result.hitPos == Nz::Vector2f(0.f, i * 10.f));
					CHECK(result.hitNormal == -Nz::Vector2f::UnitY());
				}
			}
		}

		WHEN("We ask for a region")
		{
			std::vector<Nz::RigidBody2D*> results;
			world.RegionQuery(Nz::Rectf(-5.f, -5.f, 5.f, 5.f), collisionGroup, categoryMask, collisionMask, &results);

			THEN("It should be the one on the origin")
			{
				REQUIRE(results.size() == 1);
				CHECK(results[0] == &bodies[0]);
			}
		}
	}

	GIVEN("Three entities, a character, a wall and a trigger zone")
	{
		unsigned int CHARACTER_COLLISION_ID = 1;
		unsigned int WALL_COLLISION_ID = 2;
		unsigned int TRIGGER_COLLISION_ID = 3;

		Nz::PhysWorld2D world;

		Nz::Rectf characterAABB(0.f, 0.f, 1.f, 1.f);
		Nz::Collider2DRef characterBox = Nz::BoxCollider2D::New(characterAABB);
		characterBox->SetCollisionId(CHARACTER_COLLISION_ID);
		Nz::RigidBody2D character(&world, 1.f, characterBox);
		character.SetPosition(Nz::Vector2f::Zero());

		Nz::Rectf wallAABB(0.f, 0.f, 1.f, 2.f);
		Nz::Collider2DRef wallBox = Nz::BoxCollider2D::New(wallAABB);
		wallBox->SetCollisionId(WALL_COLLISION_ID);
		Nz::RigidBody2D wall(&world, 0.f, wallBox);
		wall.SetPosition(Nz::Vector2f(5.f, 0.f));

		Nz::Rectf triggerAABB(0.f, 0.f, 1.f, 1.f);
		Nz::Collider2DRef triggerBox = Nz::BoxCollider2D::New(triggerAABB);
		triggerBox->SetTrigger(true);
		triggerBox->SetCollisionId(TRIGGER_COLLISION_ID);
		Nz::RigidBody2D trigger(&world, 0.f, triggerBox);
		trigger.SetPosition(Nz::Vector2f(2.f, 0.f));

		world.Step(0.f);

		int statusTriggerCollision = 0;
		Nz::PhysWorld2D::Callback characterTriggerCallback;
		characterTriggerCallback.startCallback = [&](Nz::PhysWorld2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) -> bool {
			statusTriggerCollision = statusTriggerCollision | 1 << 0;
			return true;
		};
		characterTriggerCallback.preSolveCallback = [&](Nz::PhysWorld2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) -> bool {
			statusTriggerCollision = statusTriggerCollision | 1 << 1;
			return true;
		};
		characterTriggerCallback.postSolveCallback = [&](Nz::PhysWorld2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) {
			statusTriggerCollision = statusTriggerCollision | 1 << 2;
		};
		characterTriggerCallback.endCallback = [&](Nz::PhysWorld2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) {
			statusTriggerCollision = statusTriggerCollision | 1 << 3;
		};
		world.RegisterCallbacks(CHARACTER_COLLISION_ID, TRIGGER_COLLISION_ID, characterTriggerCallback);

		int statusWallCollision = 0;
		Nz::PhysWorld2D::Callback characterWallCallback;
		characterWallCallback.startCallback = [&](Nz::PhysWorld2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) -> bool {
			statusWallCollision = statusWallCollision | 1 << 0;
			return true;
		};
		characterWallCallback.endCallback = [&](Nz::PhysWorld2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) {
			statusWallCollision = statusWallCollision | 1 << 1;
		};
		world.RegisterCallbacks(CHARACTER_COLLISION_ID, WALL_COLLISION_ID, characterWallCallback);

		WHEN("We make our character go towards the wall")
		{
			character.SetVelocity(Nz::Vector2f(1.f, 0.f));
			for (int i = 0; i != 11; ++i)
				world.Step(0.1f);

			THEN("It should trigger several collisions")
			{
				CHECK(statusTriggerCollision == 3);
				for (int i = 0; i != 20; ++i)
					world.Step(0.1f);
				CHECK(statusTriggerCollision == 11);

				CHECK(character.GetPosition().x == Approx(3.1f).epsilon(0.01f));

				for (int i = 0; i != 9; ++i)
					world.Step(0.1f);
				CHECK(character.GetPosition().x == Approx(4.f).epsilon(0.01f));
				world.Step(0.1f);
				CHECK(character.GetPosition().x == Approx(4.f).epsilon(0.01f));
				CHECK(statusWallCollision == 1); // It should be close to the wall

				character.SetVelocity(Nz::Vector2f(-2.f, 0.f));
				for (int i = 0; i != 10; ++i)
					world.Step(0.1f);
				CHECK(statusWallCollision == 3);
			}
		}
	}
}

Nz::RigidBody2D CreateBody(Nz::PhysWorld2D& world, const Nz::Vector2f& position, bool isMoving, const Nz::Vector2f& lengths)
{
	Nz::Rectf aabb(0.f, 0.f, lengths.x, lengths.y);
	Nz::Collider2DRef box = Nz::BoxCollider2D::New(aabb);
	box->SetCategoryMask(categoryMask);
	box->SetCollisionMask(collisionMask);
	float mass = isMoving ? 1.f : 0.f;
	Nz::RigidBody2D rigidBody(&world, mass, box);
	rigidBody.SetPosition(position);
	return rigidBody;
}
