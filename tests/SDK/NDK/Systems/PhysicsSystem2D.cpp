#include <NDK/Systems/PhysicsSystem2D.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <Catch/catch.hpp>

Ndk::EntityHandle CreateBaseEntity(Ndk::World& world, const Nz::Vector2f& position, const Nz::Rectf AABB);

SCENARIO("PhysicsSystem2D", "[NDK][PHYSICSSYSTEM2D]")
{
	GIVEN("A world and an entity")
	{
		Ndk::World world;

		Nz::Vector2f position(2.f, 3.f);
		Nz::Rectf movingAABB(0.f, 0.f, 16.f, 18.f);
		Ndk::EntityHandle movingEntity = CreateBaseEntity(world, position, movingAABB);
		Ndk::NodeComponent& nodeComponent = movingEntity->GetComponent<Ndk::NodeComponent>();
		Ndk::PhysicsComponent2D& physicsComponent2D = movingEntity->AddComponent<Ndk::PhysicsComponent2D>();

		world.GetSystem<Ndk::PhysicsSystem2D>().SetFixedUpdateRate(30.f);

		WHEN("We update the world")
		{
			world.Update(1.f);

			THEN("Entity should have correct bounding box")
			{
				REQUIRE(nodeComponent.GetPosition() == position);
				movingAABB.Translate(position);
				REQUIRE(physicsComponent2D.GetAABB() == movingAABB);
			}
		}

		WHEN("We make it collide with a wall")
		{
			int rawDistance = 3;
			Nz::Vector2f distance(rawDistance, 0.f);
			Nz::Vector2f wallPosition = position + Nz::Vector2f(movingAABB.width, 0.f) + distance;
			Nz::Rectf wallAABB(0.f, 0.f, 100.f, 100.f);
			Ndk::EntityHandle wallEntity = CreateBaseEntity(world, wallPosition, wallAABB);

			world.Update(1.f);

			THEN("It should moved freely")
			{
				REQUIRE(nodeComponent.GetPosition() == position);
				movingAABB.Translate(position);
				REQUIRE(physicsComponent2D.GetAABB() == movingAABB);

				physicsComponent2D.SetVelocity(Nz::Vector2f::UnitX());

				for (int i = 0; i < rawDistance; ++i)
				{
					world.Update(1.f);
					position += Nz::Vector2f::UnitX();
					REQUIRE(nodeComponent.GetPosition() == position);
					movingAABB.Translate(Nz::Vector2f::UnitX());
					REQUIRE(physicsComponent2D.GetAABB() == movingAABB);
				}
			}

			AND_THEN("It should be stopped by it")
			{
				world.Update(1.f);
				REQUIRE(nodeComponent.GetPosition().SquaredDistance(position) < 0.1f);
			}
		}
	}

	GIVEN("A world and a simple entity")
	{
		Ndk::World world;

		Nz::Vector2f position(0.f, 0.f);
		Nz::Rectf movingAABB(0.f, 0.f, 1.f, 2.f);
		Ndk::EntityHandle movingEntity = CreateBaseEntity(world, position, movingAABB);
		Ndk::NodeComponent& nodeComponent = movingEntity->GetComponent<Ndk::NodeComponent>();
		Ndk::PhysicsComponent2D& physicsComponent2D = movingEntity->AddComponent<Ndk::PhysicsComponent2D>();

		world.GetSystem<Ndk::PhysicsSystem2D>().SetFixedUpdateRate(30.f);

		WHEN("We make rotate our entity")
		{
			float angularSpeed = Nz::FromDegrees(45.f);
			physicsComponent2D.SetAngularVelocity(angularSpeed);
			world.Update(2.f);

			THEN("It should have been rotated")
			{
				CHECK(physicsComponent2D.GetAngularVelocity() == Approx(angularSpeed));
				CHECK(physicsComponent2D.GetAABB() == Nz::Rectf(-2.f, 0.f, 2.f, 1.f));
				CHECK(physicsComponent2D.GetRotation() == Approx(Nz::FromDegrees(90.f)));
				CHECK(nodeComponent.GetRotation().ToEulerAngles().roll == Approx(Nz::FromDegrees(90.f)));
			}
		}

		WHEN("We put a force on it")
		{
			float stepSize = world.GetSystem<Ndk::PhysicsSystem2D>().GetWorld().GetStepSize();
			Nz::Vector2f velocity = Nz::Vector2f::UnitX();
			physicsComponent2D.AddForce(velocity / stepSize);
			world.Update(1.f);

			THEN("Velocity should be the one targetted")
			{
				REQUIRE(physicsComponent2D.GetVelocity() == velocity);
				world.Update(99.f);
				REQUIRE(physicsComponent2D.GetPosition().Distance(Nz::Vector2f::UnitX() * 100.f) < 1.f);
				REQUIRE(nodeComponent.GetPosition().Distance(Nz::Vector2f::UnitX() * 100.f) < 1.f);
			}
		}
	}

	GIVEN("A world and a simple entity not at the origin")
	{
		Ndk::World world;

		Nz::Vector2f position(3.f, 4.f);
		Nz::Rectf movingAABB(0.f, 0.f, 1.f, 2.f);
		Ndk::EntityHandle movingEntity = CreateBaseEntity(world, position, movingAABB);
		Ndk::NodeComponent& nodeComponent = movingEntity->GetComponent<Ndk::NodeComponent>();
		Ndk::PhysicsComponent2D& physicsComponent2D = movingEntity->AddComponent<Ndk::PhysicsComponent2D>();

		world.GetSystem<Ndk::PhysicsSystem2D>().SetFixedUpdateRate(30.f);

		WHEN("We make rotate our entity")
		{
			float angularSpeed = Nz::FromDegrees(45.f);
			physicsComponent2D.SetAngularVelocity(angularSpeed);
			world.Update(2.f);

			THEN("It should have been rotated")
			{
				CHECK(physicsComponent2D.GetAngularVelocity() == angularSpeed);
				CHECK(physicsComponent2D.GetAABB() == Nz::Rectf(1.f, 4.f, 2.f, 1.f));
				CHECK(physicsComponent2D.GetRotation() == Approx(Nz::FromDegrees(90.f)));
				CHECK(nodeComponent.GetPosition() == position);
				CHECK(nodeComponent.GetRotation().ToEulerAngles().roll == Approx(Nz::FromDegrees(90.f)));
			}
		}
	}
}

Ndk::EntityHandle CreateBaseEntity(Ndk::World& world, const Nz::Vector2f& position, const Nz::Rectf AABB)
{
	Ndk::EntityHandle entity = world.CreateEntity();
	Ndk::NodeComponent& nodeComponent = entity->AddComponent<Ndk::NodeComponent>();
	nodeComponent.SetPosition(position);
	Nz::BoxCollider2DRef collisionBox = Nz::BoxCollider2D::New(AABB);
	entity->AddComponent<Ndk::CollisionComponent2D>(collisionBox);
	return entity;
}
