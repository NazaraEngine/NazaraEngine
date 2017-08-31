#include <NDK/World.hpp>
#include <NDK/Component.hpp>
#include <Catch/catch.hpp>

namespace
{
	class UpdatableComponent : public Ndk::Component<UpdatableComponent>
	{
		public:
			bool IsUpdated()
			{
				return m_updated;
			}

			void SetUpdated()
			{
				m_updated = true;
			}

			static Ndk::ComponentIndex componentIndex;

		private:
			bool m_updated = false;
	};

	Ndk::ComponentIndex UpdatableComponent::componentIndex;

	class UpdateSystem : public Ndk::System<UpdateSystem>
	{
		public:
			UpdateSystem()
			{
				Requires<UpdatableComponent>();
			}

			~UpdateSystem() = default;

			static Ndk::SystemIndex systemIndex;

		private:
			void OnUpdate(float /*elapsedTime*/) override
			{
				for (const Ndk::EntityHandle& entity : GetEntities())
				{
					UpdatableComponent& updatable = entity->GetComponent<UpdatableComponent>();
					updatable.SetUpdated();
				}
			}
	};

	Ndk::SystemIndex UpdateSystem::systemIndex;
}

SCENARIO("World", "[NDK][WORLD]")
{
	GIVEN("A brave new world and the update system")
	{
		Ndk::World world;
		Ndk::BaseSystem& system = world.AddSystem<UpdateSystem>();

		WHEN("We had a new entity with an updatable component and a system")
		{
			const Ndk::EntityHandle& entity = world.CreateEntity();
			UpdatableComponent& component = entity->AddComponent<UpdatableComponent>();

			THEN("We can get our entity and our system")
			{
				const Ndk::EntityHandle& fetchedEntity = world.GetEntity(entity->GetId());
				REQUIRE(fetchedEntity->GetWorld() == &world);
			}

			THEN("We can clone it")
			{
				const Ndk::EntityHandle& clone = world.CloneEntity(entity->GetId());
				REQUIRE(world.IsEntityValid(clone));
			}
		}

		AND_WHEN("We update our world with our entity")
		{
			REQUIRE(&world.GetSystem(UpdateSystem::systemIndex) == &world.GetSystem<UpdateSystem>());
			Ndk::EntityHandle entity = world.CreateEntity();
			UpdatableComponent& component = entity->AddComponent<UpdatableComponent>();

			THEN("Our entity component must be updated")
			{
				world.Update(1.f);

				REQUIRE(component.IsUpdated());
			}

			THEN("We kill our entity")
			{
				REQUIRE(entity->IsValid());

				world.KillEntity(entity);
				world.Update(1.f);

				REQUIRE(!world.IsEntityValid(entity));
			}
		}
	}
}