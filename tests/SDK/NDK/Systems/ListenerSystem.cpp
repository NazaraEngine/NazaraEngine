#include <NDK/Systems/ListenerSystem.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/ListenerComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Catch/catch.hpp>

SCENARIO("ListenerSystem", "[NDK][LISTENERSYSTEM]")
{
	GIVEN("A world and an entity with listener & node components")
	{
		Ndk::World world;
		Ndk::EntityHandle entity = world.CreateEntity();
		Ndk::ListenerComponent& listenerComponent = entity->AddComponent<Ndk::ListenerComponent>();
		Ndk::NodeComponent& nodeComponent = entity->AddComponent<Ndk::NodeComponent>();

		WHEN("We move our entity")
		{
			Nz::Vector3f position = Nz::Vector3f::Unit() * 3.f;
			nodeComponent.SetPosition(position);
			Nz::Quaternionf rotation = Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), Nz::Vector3f::Up());
			nodeComponent.SetRotation(rotation);
			world.Update(1.f);

			THEN("Our listener should have moved")
			{
				REQUIRE(Nz::Audio::GetListenerPosition() == position);
				REQUIRE(Nz::Audio::GetListenerRotation() == rotation);
			}

			THEN("With a component of velocity")
			{
				Ndk::VelocityComponent& velocityComponent = entity->AddComponent<Ndk::VelocityComponent>();
				Nz::Vector3f velocity = Nz::Vector3f::Unit() * 2.f;
				velocityComponent.linearVelocity = velocity;

				world.Update(1.f);
				REQUIRE(Nz::Audio::GetListenerVelocity() == velocity);
			}
		}
	}
}