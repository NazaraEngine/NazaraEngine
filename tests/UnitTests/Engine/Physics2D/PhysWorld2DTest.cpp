#include <Nazara/Physics2D/PhysWorld2D.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

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

		world.Step(Nz::Time::Second());

		WHEN("We ask for the nearest body")
		{
			Nz::PhysWorld2D::NearestQueryResult result;
			REQUIRE(world.NearestBodyQuery(-Nz::Vector2f::UnitY() * 1.f, 2.f, collisionGroup, categoryMask, collisionMask, &result));

			THEN("It should be the one on the origin")
			{
				CHECK(result.nearestBody == &bodies[0]);
				CHECK(result.closestPoint.ApproxEqual(Nz::Vector2f::Zero()));
				CHECK(result.fraction.ApproxEqual(-Nz::Vector2f::UnitY()));
				CHECK(result.distance == Catch::Approx(1.f));
			}

			REQUIRE(world.NearestBodyQuery(Nz::Vector2f::UnitY() * 2.f, 2.f, collisionGroup, categoryMask, collisionMask, &result));

			THEN("It should be the one on the origin")
			{
				CHECK(result.nearestBody == &bodies[0]);
				CHECK(result.closestPoint.ApproxEqual(Nz::Vector2f::UnitY()));
				CHECK(result.fraction.ApproxEqual(Nz::Vector2f::UnitY()));
				CHECK(result.distance == Catch::Approx(1.f));
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
				CHECK(result.fraction == Catch::Approx(1.f / 42.f));
				CHECK(result.hitPos.ApproxEqual(Nz::Vector2f::Zero()));
				CHECK(result.hitNormal.ApproxEqual(-Nz::Vector2f::UnitY()));
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
					CHECK(result.fraction == Catch::Approx(i / 4.f).margin(0.1f));
					CHECK(result.hitPos.ApproxEqual(Nz::Vector2f(0.f, i * 10.f), 0.0001f));
					CHECK(result.hitNormal.ApproxEqual(-Nz::Vector2f::UnitY(), 0.0001f));
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

		int statusTriggerCollision = 0;

		Nz::PhysWorld2D world;

		Nz::Rectf characterAABB(0.f, 0.f, 1.f, 1.f);
		std::shared_ptr<Nz::Collider2D> characterBox = std::make_shared<Nz::BoxCollider2D>(characterAABB);
		characterBox->SetCollisionId(CHARACTER_COLLISION_ID);
		Nz::RigidBody2D character(world, Nz::RigidBody2D::DynamicSettings(characterBox, 1.f));
		character.SetPosition(Nz::Vector2f::Zero());

		Nz::Rectf wallAABB(0.f, 0.f, 1.f, 2.f);
		std::shared_ptr<Nz::Collider2D> wallBox = std::make_shared<Nz::BoxCollider2D>(wallAABB);
		wallBox->SetCollisionId(WALL_COLLISION_ID);
		Nz::RigidBody2D wall(world, Nz::RigidBody2D::StaticSettings(wallBox));
		wall.SetPosition(Nz::Vector2f(5.f, 0.f));

		Nz::Rectf triggerAABB(0.f, 0.f, 1.f, 1.f);
		std::shared_ptr<Nz::Collider2D> triggerBox = std::make_shared<Nz::BoxCollider2D>(triggerAABB);
		triggerBox->SetTrigger(true);
		triggerBox->SetCollisionId(TRIGGER_COLLISION_ID);
		Nz::RigidBody2D trigger(world, Nz::RigidBody2D::StaticSettings(triggerBox));
		trigger.SetPosition(Nz::Vector2f(2.f, 0.f));

		world.Step(Nz::Time::Zero());

		Nz::PhysWorld2D::ContactCallbacks characterTriggerCallback;
		characterTriggerCallback.startCallback = [&](Nz::PhysWorld2D&, Nz::PhysArbiter2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) -> bool {
			statusTriggerCollision = statusTriggerCollision | 1 << 0;
			return true;
		};
		characterTriggerCallback.preSolveCallback = [&](Nz::PhysWorld2D&, Nz::PhysArbiter2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) -> bool {
			statusTriggerCollision = statusTriggerCollision | 1 << 1;
			return true;
		};
		characterTriggerCallback.postSolveCallback = [&](Nz::PhysWorld2D&, Nz::PhysArbiter2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) {
			statusTriggerCollision = statusTriggerCollision | 1 << 2;
		};
		characterTriggerCallback.endCallback = [&](Nz::PhysWorld2D&, Nz::PhysArbiter2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) {
			statusTriggerCollision = statusTriggerCollision | 1 << 3;
		};
		world.RegisterCallbacks(CHARACTER_COLLISION_ID, TRIGGER_COLLISION_ID, characterTriggerCallback);

		int statusWallCollision = 0;
		Nz::PhysWorld2D::ContactCallbacks characterWallCallback;
		characterWallCallback.startCallback = [&](Nz::PhysWorld2D&, Nz::PhysArbiter2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) -> bool {
			statusWallCollision = statusWallCollision | 1 << 0;
			return true;
		};
		characterWallCallback.endCallback = [&](Nz::PhysWorld2D&, Nz::PhysArbiter2D&, Nz::RigidBody2D&, Nz::RigidBody2D&, void*) {
			statusWallCollision = statusWallCollision | 1 << 1;
		};
		world.RegisterCallbacks(CHARACTER_COLLISION_ID, WALL_COLLISION_ID, characterWallCallback);

		WHEN("We make our character go towards the wall")
		{
			character.SetVelocity(Nz::Vector2f(1.f, 0.f));
			for (int i = 0; i != 11; ++i)
				world.Step(Nz::Time::TickDuration(10));

			THEN("It should trigger several collisions")
			{
				CHECK(statusTriggerCollision == 3);
				for (int i = 0; i != 20; ++i)
					world.Step(Nz::Time::TickDuration(10));
				CHECK(statusTriggerCollision == 11);

				CHECK(character.GetPosition().x == Catch::Approx(3.1f).margin(0.01f));

				for (int i = 0; i != 9; ++i)
					world.Step(Nz::Time::TickDuration(10));

				CHECK(character.GetPosition().x == Catch::Approx(4.f).margin(0.01f));
				world.Step(Nz::Time::TickDuration(10));
				CHECK(character.GetPosition().x == Catch::Approx(4.f).margin(0.01f));
				CHECK(statusWallCollision == 1); // It should be close to the wall

				character.SetVelocity(Nz::Vector2f(-2.f, 0.f));
				for (int i = 0; i != 10; ++i)
					world.Step(Nz::Time::TickDuration(10));

				CHECK(statusWallCollision == 3);
			}
		}
	}
}

Nz::RigidBody2D CreateBody(Nz::PhysWorld2D& world, const Nz::Vector2f& position, bool isMoving, const Nz::Vector2f& lengths)
{
	Nz::Rectf aabb(0.f, 0.f, lengths.x, lengths.y);
	std::shared_ptr<Nz::Collider2D> box = std::make_shared<Nz::BoxCollider2D>(aabb);
	box->SetCategoryMask(categoryMask);
	box->SetCollisionMask(collisionMask);

	Nz::RigidBody2D::DynamicSettings settings;
	settings.collider = std::move(box);
	settings.mass = isMoving ? 1.f : 0.f;
	settings.position = position;

	return Nz::RigidBody2D(world, settings);
}
