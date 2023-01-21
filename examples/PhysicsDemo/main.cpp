#include <Nazara/Core.hpp>
#include <Nazara/Core/AppEntitySystemComponent.hpp>
#include <Nazara/Core/Systems.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Physics3D/Components.hpp>
#include <Nazara/Physics3D/Systems.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <entt/entt.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() != 'n')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	Nz::Application<Nz::Graphics, Nz::Physics3D> app(rendererConfig);

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();
	Nz::Physics3DSystem& physSytem = ecs.AddSystem<Nz::Physics3DSystem>();
	Nz::RenderSystem& renderSystem = ecs.AddSystem<Nz::RenderSystem>();

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
		NazaraError("Failed to load model");
		return __LINE__;
	}

	const Nz::Boxf& spaceshipAABB = spaceshipMesh->GetAABB();
	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*spaceshipMesh);

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	std::shared_ptr<Nz::MaterialInstance> material = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
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

	std::shared_ptr<Nz::Model> model = std::make_shared<Nz::Model>(std::move(gfxMesh), spaceshipAABB);
	for (std::size_t i = 0; i < model->GetSubMeshCount(); ++i)
		model->SetMaterial(i, material);

	std::shared_ptr<Nz::MaterialInstance> textMaterial = Nz::Graphics::Instance()->GetDefaultMaterials().basicTransparent->Clone();
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

	entt::handle viewer = ecs.CreateEntity();
	{
		viewer.emplace<Nz::NodeComponent>();
		auto& cameraComponent = viewer.emplace<Nz::CameraComponent>(&windowSwapchain.GetSwapchain());
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	auto shipCollider = std::make_shared<Nz::ConvexCollider3D>(vertices, vertexMapper.GetVertexCount(), 0.01f);

	std::shared_ptr<Nz::MaterialInstance> colliderMat = Nz::Graphics::Instance()->GetDefaultMaterials().basicMaterial->Instantiate();
	colliderMat->SetValueProperty("BaseColor", Nz::Color::Green());
	for (std::string_view passName : { "DepthPass", "ForwardPass" })
	{
		colliderMat->UpdatePassStates(passName, [](Nz::RenderStates& states)
		{
			states.primitiveMode = Nz::PrimitiveMode::LineList;
			return true;
		});
	}

	std::shared_ptr<Nz::Model> colliderModel;
	{
		std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(shipCollider->GenerateMesh());
		std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

		colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh, spaceshipAABB);
		for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
			colliderModel->SetMaterial(i, colliderMat);
	}

	entt::handle textEntity = ecs.CreateEntity();
	{
		auto& entityGfx = textEntity.emplace<Nz::GraphicsComponent>();
		entityGfx.AttachRenderable(sprite, 1);

		auto& entityNode = textEntity.emplace<Nz::NodeComponent>();
		entityNode.SetPosition(0.f, 5.f, 0.f);
	}
	entt::handle playerEntity = ecs.CreateEntity();

	entt::handle headingEntity = ecs.CreateEntity();
	{
		auto& entityLight = playerEntity.emplace<Nz::LightComponent>();
		auto& spotLight = entityLight.AddLight<Nz::SpotLight>(1);
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateShadowMapSize(1024);

		auto& entityGfx = playerEntity.emplace<Nz::GraphicsComponent>();
		entityGfx.AttachRenderable(model, 1);

		auto& entityNode = playerEntity.emplace<Nz::NodeComponent>();
		entityNode.SetPosition(Nz::Vector3f(12.5f, 0.f, 25.f));

		auto& entityPhys = playerEntity.emplace<Nz::RigidBody3DComponent>(physSytem.CreateRigidBody(shipCollider));
		entityPhys.SetMass(50.f);
		entityPhys.SetAngularDamping(Nz::Vector3f::Zero());

		auto& headingNode = headingEntity.emplace<Nz::NodeComponent>();
		headingNode.SetInheritRotation(false);
		headingNode.SetParent(entityNode);
	}

	viewer.get<Nz::NodeComponent>().SetParent(headingEntity);
	viewer.get<Nz::NodeComponent>().SetPosition(Nz::Vector3f::Backward() * 2.5f + Nz::Vector3f::Up() * 1.f);

	for (std::size_t x = 0; x < 3; ++x)
	{
		for (std::size_t y = 0; y < 3; ++y)
		{
			for (std::size_t z = 0; z < 3; ++z)
			{
				entt::handle entity = ecs.CreateEntity();
				auto& entityGfx = entity.emplace<Nz::GraphicsComponent>();
				entityGfx.AttachRenderable(model, 1);

				auto& entityNode = entity.emplace<Nz::NodeComponent>();
				entityNode.SetPosition(Nz::Vector3f(x * 2.f, y * 1.5f, z * 2.f));
				entityNode.SetRotation(Nz::EulerAnglesf(0.f, Nz::TurnAnglef(0.5f), 0.f));

				auto& entityPhys = entity.emplace<Nz::RigidBody3DComponent>(physSytem.CreateRigidBody(shipCollider));
				entityPhys.SetMass(1.f);
				entityPhys.SetAngularDamping(Nz::Vector3f::Zero());
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
				auto view = ecs.GetRegistry().view<Nz::GraphicsComponent, Nz::RigidBody3DComponent>();
				for (auto [entity, gfxComponent, _] : view.each())
					gfxComponent.AttachRenderable(colliderModel, 1);
			}
			else
			{
				auto view = ecs.GetRegistry().view<Nz::GraphicsComponent, Nz::RigidBody3DComponent>();
				for (auto [entity, gfxComponent, _] : view.each())
					gfxComponent.DetachRenderable(colliderModel);
			}
		}
		else if (event.virtualKey == Nz::Keyboard::VKey::Space)
		{
			entt::handle entity = ecs.CreateEntity();
			auto& entityGfx = entity.emplace<Nz::GraphicsComponent>();
			entityGfx.AttachRenderable(model, 1);
			if (showColliders)
				entityGfx.AttachRenderable(colliderModel, 1);

			entity.emplace<Nz::NodeComponent>();

			auto& entityPhys = entity.emplace<Nz::RigidBody3DComponent>(physSytem.CreateRigidBody(shipCollider));
			entityPhys.SetMass(1.f);
			entityPhys.SetAngularDamping(Nz::Vector3f::Zero());
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

	app.AddUpdater([&](Nz::Time elapsedTime)
	{
		if (std::optional<Nz::Time> deltaTime = updateClock.RestartIfOver(Nz::Time::TickDuration(60)))
		{
			float elapsedTime = deltaTime->AsSeconds();

			auto spaceshipView = ecs.GetRegistry().view<Nz::NodeComponent, Nz::RigidBody3DComponent>();
			for (auto&& [entity, node, _] : spaceshipView.each())
			{
				if (entity == playerEntity)
					continue;

				Nz::Vector3f spaceshipPos = node.GetPosition(Nz::CoordSys::Global);
				if (spaceshipPos.GetSquaredLength() > Nz::IntegralPow(20.f, 2))
					ecs.GetRegistry().destroy(entity);
			}

			Nz::RigidBody3DComponent& playerShipBody = playerEntity.get<Nz::RigidBody3DComponent>();
			Nz::Quaternionf currentRotation = playerShipBody.GetRotation();

			Nz::Vector3f desiredHeading = headingEntity.get<Nz::NodeComponent>().GetForward();
			Nz::Vector3f currentHeading = currentRotation * Nz::Vector3f::Forward();
			Nz::Vector3f headingError = currentHeading.CrossProduct(desiredHeading);

			Nz::Vector3f desiredUp = headingEntity.get<Nz::NodeComponent>().GetUp();
			Nz::Vector3f currentUp = currentRotation * Nz::Vector3f::Up();
			Nz::Vector3f upError = currentUp.CrossProduct(desiredUp);

			playerShipBody.AddTorque(headingController.Update(headingError, elapsedTime) * 10.f);
			playerShipBody.AddTorque(upController.Update(upError, elapsedTime) * 10.f);

			float mass = playerShipBody.GetMass();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				playerShipBody.AddForce(Nz::Vector3f::Forward() * 2.5f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				playerShipBody.AddForce(Nz::Vector3f::Backward() * 2.5f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				playerShipBody.AddForce(Nz::Vector3f::Left() * 2.5f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				playerShipBody.AddForce(Nz::Vector3f::Right() * 2.5f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				playerShipBody.AddForce(Nz::Vector3f::Up() * 3.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				playerShipBody.AddForce(Nz::Vector3f::Down() * 3.f * mass, Nz::CoordSys::Local);
		}

		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(ecs.GetRegistry().alive()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
}
