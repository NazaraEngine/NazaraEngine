#include <NDK/Systems/PhysicsSystem.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <Catch/catch.hpp>

SCENARIO("PhysicsSystem", "[NDK][PHYSICSSYSTEM]")
{
	GIVEN("A world and a static entity & a dynamic entity")
	{
		Ndk::World world;
		const Ndk::EntityHandle& staticEntity = world.CreateEntity();
		Ndk::CollisionComponent& collisionComponentStatic = staticEntity->AddComponent<Ndk::CollisionComponent>();
		Ndk::NodeComponent& nodeComponentStatic = staticEntity->AddComponent<Ndk::NodeComponent>();

		const Ndk::EntityHandle& dynamicEntity = world.CreateEntity();
		Ndk::NodeComponent& nodeComponentDynamic = dynamicEntity->AddComponent<Ndk::NodeComponent>();
		Ndk::PhysicsComponent& physicsComponentDynamic = dynamicEntity->AddComponent<Ndk::PhysicsComponent>();

		WHEN("We make collide these two entities")
		{
			nodeComponentDynamic.SetPosition(-Nz::Vector3f::UnitZ());
			physicsComponentDynamic.AddForce(Nz::Vector3f::UnitZ());

			THEN("The dynamic entity should have hit the static one")
			{
				world.Update(1.f); // On origin
				world.Update(1.f); // On origin due to collision
				REQUIRE(nodeComponentStatic.GetPosition().SquaredDistance(nodeComponentDynamic.GetPosition()) < 0.2f);
			}
		}
	}
}