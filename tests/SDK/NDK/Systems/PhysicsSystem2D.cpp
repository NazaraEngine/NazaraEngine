#include <NDK/Systems/PhysicsSystem2D.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <Catch/catch.hpp>

SCENARIO("PhysicsSystem2D", "[NDK][PHYSICSSYSTEM2D]")
{
	GIVEN("A world and an entity")
	{
		Ndk::World world;
		const Ndk::EntityHandle& entity = world.CreateEntity();
		Ndk::NodeComponent& nodeComponent = entity->AddComponent<Ndk::NodeComponent>();
		Nz::Vector2f position(2.f, 3.f);
		nodeComponent.SetPosition(position);
		Nz::Rectf aabb(0.f, 0.f, 16.f, 18.f);
		Nz::BoxCollider2DRef collisionBox = Nz::BoxCollider2D::New(aabb);
		Ndk::CollisionComponent2D& collisionComponent = entity->AddComponent<Ndk::CollisionComponent2D>(collisionBox);
		Ndk::PhysicsComponent2D& physicsComponent = entity->AddComponent<Ndk::PhysicsComponent2D>();

		WHEN("We update the world")
		{
			world.Update(1.f);

			THEN("Entity should have correct bounding box")
			{
				REQUIRE(nodeComponent.GetPosition() == position);
				aabb.Translate(position);
				REQUIRE(physicsComponent.GetAABB() == aabb);
			}
		}
	}
}