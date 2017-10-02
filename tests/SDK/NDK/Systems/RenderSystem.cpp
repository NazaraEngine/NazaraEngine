#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/World.hpp>
#include <NDK/Components.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Catch/catch.hpp>

void CompareAABB(const Nz::Rectf& aabb, const Nz::BoundingVolumef& boundingVolume);

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

	GIVEN("A world with 2D coordinates (upper-left) and an entity with graphics and physics")
	{
		Ndk::World world;
		world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
		const Ndk::EntityHandle& entity = world.CreateEntity();

		Nz::Vector2f position(3.f, 4.f);
		Ndk::NodeComponent& nodeComponent = entity->AddComponent<Ndk::NodeComponent>();
		nodeComponent.SetPosition(position);

		Nz::Vector2f dimensions(1.f, 2.f);
		Ndk::GraphicsComponent& graphicsComponent = entity->AddComponent<Ndk::GraphicsComponent>();
		Nz::SpriteRef sprite = Nz::Sprite::New();
		sprite->SetSize(dimensions);
		graphicsComponent.Attach(sprite);

		Nz::Rectf aabb(Nz::Vector2f::Zero(), dimensions);
		Nz::BoxCollider2DRef boxCollider2D = Nz::BoxCollider2D::New(aabb);
		entity->AddComponent<Ndk::CollisionComponent2D>(boxCollider2D);
		Ndk::PhysicsComponent2D& physicsComponent2D = entity->AddComponent<Ndk::PhysicsComponent2D>();

		world.Update(1.f);

		WHEN("We move it")
		{
			Nz::Vector2f velocity = Nz::Vector2f::UnitY();
			physicsComponent2D.SetVelocity(velocity);
			world.Update(1.f);

			THEN("Graphics and physics should be synchronised")
			{
				CHECK(nodeComponent.GetPosition() == position + velocity);
				CHECK(physicsComponent2D.GetAABB() == aabb.Translate(position + velocity));
				CompareAABB(physicsComponent2D.GetAABB(), graphicsComponent.GetBoundingVolume());
			}
		}

		WHEN("We set an angular velocity")
		{
			float angularSpeed = Nz::FromDegrees(90.f);
			physicsComponent2D.SetAngularVelocity(angularSpeed);
			world.Update(1.f);

			THEN("We expect those to be true")
			{
				CHECK(physicsComponent2D.GetAngularVelocity() == Approx(angularSpeed));
				CHECK(physicsComponent2D.GetRotation() == Approx(angularSpeed));
				CHECK(physicsComponent2D.GetAABB() == Nz::Rectf(1.f, 4.f, 2.f, 1.f));
				CompareAABB(physicsComponent2D.GetAABB(), graphicsComponent.GetBoundingVolume());

				world.Update(1.f);
				CHECK(physicsComponent2D.GetRotation() == Approx(2.f * angularSpeed));
				CHECK(physicsComponent2D.GetAABB() == Nz::Rectf(2.f, 2.f, 1.f, 2.f));
				CompareAABB(physicsComponent2D.GetAABB(), graphicsComponent.GetBoundingVolume());

				world.Update(1.f);
				CHECK(physicsComponent2D.GetRotation() == Approx(3.f * angularSpeed));
				CHECK(physicsComponent2D.GetAABB() == Nz::Rectf(3.f, 3.f, 2.f, 1.f));
				CompareAABB(physicsComponent2D.GetAABB(), graphicsComponent.GetBoundingVolume());

				world.Update(1.f);
				CHECK(physicsComponent2D.GetRotation() == Approx(4.f * angularSpeed));
			}
		}
	}
}

void CompareAABB(const Nz::Rectf& aabb, const Nz::BoundingVolumef& boundingVolume)
{
	Nz::Boxf box = boundingVolume.aabb;
	CHECK(aabb.x == Approx(box.x));
	CHECK(aabb.y == Approx(box.y));
	CHECK(aabb.width == Approx(box.width));
	CHECK(aabb.height == Approx(box.height));
}