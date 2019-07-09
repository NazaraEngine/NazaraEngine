#include <NDK/Systems/VelocitySystem.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>
#include <Catch/catch.hpp>

SCENARIO("VelocitySystem", "[NDK][VELOCITYSYSTEM]")
{
	GIVEN("A world and an entity with velocity & node components")
	{
		Ndk::World world;
		const Ndk::EntityHandle& entity = world.CreateEntity();
		Ndk::VelocityComponent& velocityComponent = entity->AddComponent<Ndk::VelocityComponent>();
		Ndk::NodeComponent& nodeComponent = entity->AddComponent<Ndk::NodeComponent>();

		world.GetSystem<Ndk::VelocitySystem>().SetFixedUpdateRate(30.f);

		WHEN("We give a speed to our entity")
		{
			Nz::Vector3f velocity = Nz::Vector3f::Unit() * 2.f;
			velocityComponent.linearVelocity = velocity;
			world.Update(1.f);

			THEN("Our entity should have moved")
			{
				REQUIRE(nodeComponent.GetPosition().SquaredDistance(velocity) < 0.2f);
			}
		}
	}
}