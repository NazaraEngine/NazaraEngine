#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/BulletPhysics3D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <entt/entt.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

int main(int argc, char* argv[])
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Application<Nz::Graphics, Nz::BulletPhysics3D> app(argc, argv);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	Nz::BulletPhysics3DSystem& physSytem = world.AddSystem<Nz::BulletPhysics3DSystem>();
	physSytem.GetPhysWorld().SetGravity(Nz::Vector3f::Zero());

	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Physics 3D";
	Nz::Window& window = windowing.CreateWindow(Nz::VideoMode(1920, 1080, 32), windowTitle);
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(window);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, 90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent);

	std::shared_ptr<Nz::Mesh> spaceshipMesh = Nz::Mesh::LoadFromFile(resourceDir / "Spaceship/spaceship.obj", meshParams);
	if (!spaceshipMesh)
	{
		NazaraError("failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*spaceshipMesh);

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	std::shared_ptr<Nz::MaterialInstance> material = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
	for (std::string_view passName : { "DepthPass", "ForwardPass" })
	{
		material->UpdatePassStates(passName, [](Nz::RenderStates& states)
		{
			states.depthClamp = true;
			return true;
		});
	}

	material->SetTextureProperty("AlphaMap", Nz::Texture::LoadFromFile(resourceDir / "alphatile.png", texParams));
	material->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams));

	std::shared_ptr<Nz::Model> model = std::make_shared<Nz::Model>(std::move(gfxMesh));
	for (std::size_t i = 0; i < model->GetSubMeshCount(); ++i)
		model->SetMaterial(i, material);

	std::shared_ptr<Nz::MaterialInstance> textMaterial = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic, Nz::MaterialInstancePreset::Transparent);
	textMaterial->UpdatePassFlags("ForwardPass", Nz::MaterialPassFlag::SortByDistance);
	textMaterial->UpdatePassStates("ForwardPass", [](Nz::RenderStates& states)
	{
		states.depthClamp = true;
		return true;
	});

	std::shared_ptr<Nz::TextSprite> sprite = std::make_shared<Nz::TextSprite>(textMaterial);
	sprite->Update(Nz::SimpleTextDrawer::Draw("Voix ambiguë d'un cœur qui, au zéphyr, préfère les jattes de kiwis", 72), 0.01f);

	Nz::VertexMapper vertexMapper(*spaceshipMesh->GetSubMesh(0));
	Nz::SparsePtr<Nz::Vector3f> vertices = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position);

	Nz::Vector2ui windowSize = window.GetSize();

	entt::handle viewer = world.CreateEntity();
	{
		viewer.emplace<Nz::NodeComponent>();
		auto& cameraComponent = viewer.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain));
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	auto shipCollider = std::make_shared<Nz::BulletConvexCollider3D>(vertices, vertexMapper.GetVertexCount());

	std::shared_ptr<Nz::MaterialInstance> colliderMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
	colliderMat->SetValueProperty("BaseColor", Nz::Color::Green());
	colliderMat->UpdatePassesStates([](Nz::RenderStates& states)
	{
		states.primitiveMode = Nz::PrimitiveMode::LineList;
		return true;
	});

	std::shared_ptr<Nz::Model> colliderModel;
	{
		std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(shipCollider->GenerateDebugMesh());
		std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

		colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
		for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
			colliderModel->SetMaterial(i, colliderMat);
	}

	entt::handle textEntity = world.CreateEntity();
	{
		auto& entityGfx = textEntity.emplace<Nz::GraphicsComponent>();
		entityGfx.AttachRenderable(sprite, 1);

		auto& entityNode = textEntity.emplace<Nz::NodeComponent>();
		entityNode.SetPosition(0.f, 5.f, 0.f);
	}
	entt::handle playerEntity = world.CreateEntity();

	entt::handle headingEntity = world.CreateEntity();
	{
		auto& entityGfx = playerEntity.emplace<Nz::GraphicsComponent>();
		entityGfx.AttachRenderable(model, 1);

		auto& entityNode = playerEntity.emplace<Nz::NodeComponent>();
		entityNode.SetPosition(Nz::Vector3f(12.5f, 0.f, 25.f));

		auto& entityPhys = playerEntity.emplace<Nz::BulletRigidBody3DComponent>(physSytem.CreateRigidBody(shipCollider));
		entityPhys.SetMass(50.f);
		entityPhys.SetAngularDamping(0.1f);
		entityPhys.SetLinearDamping(0.5f);

		auto& headingNode = headingEntity.emplace<Nz::NodeComponent>();
		headingNode.SetInheritRotation(false);
		headingNode.SetParent(entityNode);

		entt::handle lightEntity = world.CreateEntity();

		auto& lightNode = lightEntity.emplace<Nz::NodeComponent>();
		lightNode.SetParent(playerEntity);
		lightNode.SetPosition(Nz::Vector3f::Forward() * 1.f);

		auto& entityLight = lightEntity.emplace<Nz::LightComponent>();
		auto& spotLight = entityLight.AddLight<Nz::SpotLight>(1);
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateShadowMapSize(1024);

	}

	viewer.get<Nz::NodeComponent>().SetParent(headingEntity);
	viewer.get<Nz::NodeComponent>().SetPosition(Nz::Vector3f::Backward() * 2.5f + Nz::Vector3f::Up() * 1.f);

	for (std::size_t x = 0; x < 3; ++x)
	{
		for (std::size_t y = 0; y < 3; ++y)
		{
			for (std::size_t z = 0; z < 3; ++z)
			{
				entt::handle entity = world.CreateEntity();
				auto& entityGfx = entity.emplace<Nz::GraphicsComponent>();
				entityGfx.AttachRenderable(model, 1);

				auto& entityNode = entity.emplace<Nz::NodeComponent>();
				entityNode.SetPosition(Nz::Vector3f(x * 2.f, y * 1.5f, z * 2.f));
				entityNode.SetRotation(Nz::EulerAnglesf(0.f, Nz::TurnAnglef(0.5f), 0.f));

				auto& entityPhys = entity.emplace<Nz::BulletRigidBody3DComponent>(physSytem.CreateRigidBody(shipCollider));
				entityPhys.SetMass(1.f);
				entityPhys.SetAngularDamping(0.f);
				entityPhys.SetLinearDamping(0.f);
			}
		}
	}

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock updateClock;
	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

	Nz::PidController<Nz::Vector3f> headingController(0.3f, 0.f, 0.1f);
	Nz::PidController<Nz::Vector3f> upController(1.f, 0.f, 0.1f);

	bool showColliders = false;

	Nz::WindowEventHandler& eventHandler = window.GetEventHandler();
	eventHandler.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& event)
	{
		if (event.virtualKey == Nz::Keyboard::VKey::A)
		{
			bool alphaTestEnabled = std::get<bool>(*material->GetValueProperty("AlphaTest"));
			material->SetValueProperty("AlphaTest", !alphaTestEnabled);
		}
		else if (event.virtualKey == Nz::Keyboard::VKey::B)
		{
			showColliders = !showColliders;
			if (showColliders)
			{
				auto view = world.GetRegistry().view<Nz::GraphicsComponent, Nz::BulletRigidBody3DComponent>();
				for (auto [entity, gfxComponent, _] : view.each())
					gfxComponent.AttachRenderable(colliderModel, 1);
			}
			else
			{
				auto view = world.GetRegistry().view<Nz::GraphicsComponent, Nz::BulletRigidBody3DComponent>();
				for (auto [entity, gfxComponent, _] : view.each())
					gfxComponent.DetachRenderable(colliderModel);
			}
		}
		else if (event.virtualKey == Nz::Keyboard::VKey::Space)
		{
			entt::handle entity = world.CreateEntity();
			auto& entityGfx = entity.emplace<Nz::GraphicsComponent>();
			entityGfx.AttachRenderable(model, 1);
			if (showColliders)
				entityGfx.AttachRenderable(colliderModel, 1);

			entity.emplace<Nz::NodeComponent>();

			auto& entityPhys = entity.emplace<Nz::BulletRigidBody3DComponent>(physSytem.CreateRigidBody(shipCollider));
			entityPhys.SetMass(1.f);
			entityPhys.SetAngularDamping(0.f);
			entityPhys.SetLinearDamping(0.f);
		}
	});

	eventHandler.OnMouseMoved.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		float sensitivity = 0.3f;

		camAngles.yaw = camAngles.yaw - event.deltaX * sensitivity;
		camAngles.pitch = camAngles.pitch - event.deltaY * sensitivity;

		camAngles.Normalize();

		camQuat = camAngles;

		headingEntity.get<Nz::NodeComponent>().SetRotation(camQuat);
	});

	app.AddUpdaterFunc([&]
	{
		if (std::optional<Nz::Time> deltaTime = updateClock.RestartIfOver(Nz::Time::TickDuration(60)))
		{
			float elapsedTime = deltaTime->AsSeconds();

			auto spaceshipView = world.GetRegistry().view<Nz::NodeComponent, Nz::BulletRigidBody3DComponent>();
			for (auto&& [entity, node, _] : spaceshipView.each())
			{
				if (entity == playerEntity)
					continue;

				Nz::Vector3f spaceshipPos = node.GetPosition(Nz::CoordSys::Global);
				if (spaceshipPos.GetSquaredLength() > Nz::IntegralPow(20.f, 2))
					world.GetRegistry().destroy(entity);
			}

			Nz::BulletRigidBody3DComponent& playerShipBody = playerEntity.get<Nz::BulletRigidBody3DComponent>();
			Nz::Quaternionf currentRotation = playerShipBody.GetRotation();

			Nz::Vector3f desiredHeading = headingEntity.get<Nz::NodeComponent>().GetForward();
			Nz::Vector3f currentHeading = currentRotation * Nz::Vector3f::Forward();
			Nz::Vector3f headingError = currentHeading.CrossProduct(desiredHeading);

			Nz::Vector3f desiredUp = headingEntity.get<Nz::NodeComponent>().GetUp();
			Nz::Vector3f currentUp = currentRotation * Nz::Vector3f::Up();
			Nz::Vector3f upError = currentUp.CrossProduct(desiredUp);

			playerShipBody.AddTorque(headingController.Update(headingError, elapsedTime) * 200.f);
			playerShipBody.AddTorque(upController.Update(upError, elapsedTime) * 200.f);

			float mass = playerShipBody.GetMass();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				playerShipBody.AddForce(Nz::Vector3f::Forward() * 10.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				playerShipBody.AddForce(Nz::Vector3f::Backward() * 10.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				playerShipBody.AddForce(Nz::Vector3f::Left() * 10.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				playerShipBody.AddForce(Nz::Vector3f::Right() * 10.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				playerShipBody.AddForce(Nz::Vector3f::Up() * 15.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				playerShipBody.AddForce(Nz::Vector3f::Down() * 15.f * mass, Nz::CoordSys::Local);
		}

		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetAliveEntityCount()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
}
