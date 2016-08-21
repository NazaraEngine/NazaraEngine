#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/ParticleGroupComponent.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Catch/catch.hpp>

SCENARIO("RenderSystem", "[NDK][RenderSystem]")
{
	GIVEN("A world with a camera, a drawable, a light and some particles")
	{
		Ndk::World world;
		const Ndk::EntityHandle& cameraEntity = world.CreateEntity();
		Ndk::CameraComponent& cameraComponentCamera = cameraEntity->AddComponent<Ndk::CameraComponent>();
		Ndk::NodeComponent& nodeComponentCamera = cameraEntity->AddComponent<Ndk::NodeComponent>();

		const Ndk::EntityHandle& drawableEntity = world.CreateEntity();
		Ndk::GraphicsComponent& graphicsComponentDrawable = drawableEntity->AddComponent<Ndk::GraphicsComponent>();
		Nz::SpriteRef sprite = Nz::Sprite::New();
		graphicsComponentDrawable.Attach(sprite);
		Ndk::NodeComponent& nodeComponentDrawable = drawableEntity->AddComponent<Ndk::NodeComponent>();

		const Ndk::EntityHandle& lightEntity = world.CreateEntity();
		Ndk::LightComponent& lightComponentLight = lightEntity->AddComponent<Ndk::LightComponent>();
		Ndk::NodeComponent& nodeComponentLight = lightEntity->AddComponent<Ndk::NodeComponent>();

		const Ndk::EntityHandle& particlesEntity = world.CreateEntity();
		Ndk::ParticleGroupComponent& particleGroupComponentParticles = particlesEntity->AddComponent<Ndk::ParticleGroupComponent>(1, Nz::ParticleLayout_Sprite);

		WHEN("We change the render technique to ForwardRenderTechnique")
		{
			Ndk::RenderSystem& renderSystem = world.GetSystem<Ndk::RenderSystem>();
			renderSystem.ChangeRenderTechnique<Nz::ForwardRenderTechnique>();

			THEN("The render system should be ForwardRenderTechnique")
			{
				REQUIRE(renderSystem.GetRenderTechnique().GetType() == Nz::RenderTechniqueType_BasicForward);
			}
		}
	}
}