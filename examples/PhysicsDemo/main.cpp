#include <Nazara/Core.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Physics3D/Components.hpp>
#include <Nazara/Physics3D/Systems.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Shader.hpp>
#include <Nazara/Shader/SpirvConstantCache.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <entt/entt.hpp>
#include <array>
#include <iostream>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "resources";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
		resourceDir = ".." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() != 'n')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Graphics, Nz::Physics3D, Nz::ECS> nazara(rendererConfig);

	Nz::RenderWindow window;

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.storage = Nz::DataStorage::Software;
	meshParams.matrix = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, 90.f, 0.f)) * Nz::Matrix4f::Scale(Nz::Vector3f(0.002f));
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_UV);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	std::string windowTitle = "Graphics Test";
	if (!window.Create(device, Nz::VideoMode(1920, 1080, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		return __LINE__;
	}

	Nz::RenderWindowImpl* windowImpl = window.GetImpl();

	std::shared_ptr<Nz::Mesh> spaceshipMesh = Nz::Mesh::LoadFromFile(resourceDir / "Spaceship/spaceship.obj", meshParams);
	if (!spaceshipMesh)
	{
		NazaraError("Failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = std::make_shared<Nz::GraphicalMesh>(*spaceshipMesh);

	// Texture
	std::shared_ptr<Nz::Image> diffuseImage = Nz::Image::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png");
	if (!diffuseImage || !diffuseImage->Convert(Nz::PixelFormat::RGBA8_SRGB))
	{
		NazaraError("Failed to load image");
		return __LINE__;
	}

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	std::shared_ptr<Nz::Material> material = std::make_shared<Nz::Material>(Nz::BasicMaterial::GetSettings());
	material->EnableDepthBuffer(true);
	material->EnableDepthClamp(true);
	material->EnableFaceCulling(true);

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::BasicMaterial basicMat(*material);
	basicMat.EnableAlphaTest(false);
	basicMat.SetAlphaMap(Nz::Texture::LoadFromFile(resourceDir / "alphatile.png", texParams));
	basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams));
	basicMat.SetDiffuseSampler(samplerInfo);

	std::shared_ptr<Nz::Model> model = std::make_shared<Nz::Model>(std::move(gfxMesh));
	for (std::size_t i = 0; i < model->GetSubMeshCount(); ++i)
		model->SetMaterial(i, material);

	Nz::Vector2ui windowSize = window.GetSize();

	Nz::VertexMapper vertexMapper(*spaceshipMesh->GetSubMesh(0), Nz::BufferAccess::ReadOnly);
	Nz::SparsePtr<Nz::Vector3f> vertices = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position);


	entt::registry registry;

	Nz::Physics3DSystem physSytem(registry);
	Nz::RenderSystem renderSystem(registry);


	entt::entity viewer = registry.create();
	registry.emplace<Nz::NodeComponent>(viewer);
	registry.emplace<Nz::CameraComponent>(viewer, windowImpl);

	auto shipCollider = std::make_shared<Nz::ConvexCollider3D>(vertices, vertexMapper.GetVertexCount(), 0.01f);

	std::shared_ptr<Nz::Material> colliderMat = std::make_shared<Nz::Material>(Nz::BasicMaterial::GetSettings());
	colliderMat->EnableDepthBuffer(true);
	colliderMat->SetPrimitiveMode(Nz::PrimitiveMode::LineList);

	Nz::BasicMaterial colliderBasicMat(*colliderMat);
	colliderBasicMat.SetDiffuseColor(Nz::Color::Green);

	std::shared_ptr<Nz::Model> colliderModel;
	{
		std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(shipCollider->GenerateMesh());
		std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = std::make_shared<Nz::GraphicalMesh>(*colliderMesh);

		colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
		for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
			colliderModel->SetMaterial(i, colliderMat);
	}

	entt::entity playerEntity = registry.create();

	entt::entity headingEntity = registry.create();
	{
		auto& entityGfx = registry.emplace<Nz::GraphicsComponent>(playerEntity);
		entityGfx.AttachRenderable(model);

		auto& entityNode = registry.emplace<Nz::NodeComponent>(playerEntity);
		entityNode.SetPosition(Nz::Vector3f(12.5f, 0.f, 25.f));

		auto& entityPhys = registry.emplace<Nz::RigidBody3DComponent>(playerEntity, physSytem.CreateRigidBody(shipCollider));
		entityPhys.SetMass(50.f);
		entityPhys.SetAngularDamping(Nz::Vector3f::Zero());

		auto& headingNode = registry.emplace<Nz::NodeComponent>(headingEntity);
		headingNode.SetInheritRotation(false);
		headingNode.SetParent(registry, playerEntity);
	}


	registry.get<Nz::NodeComponent>(viewer).SetParent(registry, headingEntity);
	registry.get<Nz::NodeComponent>(viewer).SetPosition(Nz::Vector3f::Backward() * 2.5f + Nz::Vector3f::Up() * 1.f);

	for (std::size_t x = 0; x < 5; ++x)
	{
		for (std::size_t y = 0; y < 5; ++y)
		{
			for (std::size_t z = 0; z < 5; ++z)
			{
				entt::entity entity = registry.create();
				auto& entityGfx = registry.emplace<Nz::GraphicsComponent>(entity);
				entityGfx.AttachRenderable(model);

				auto& entityNode = registry.emplace<Nz::NodeComponent>(entity);
				entityNode.SetPosition(Nz::Vector3f(x * 2.f, y * 1.5f, z * 2.f));

				auto& entityPhys = registry.emplace<Nz::RigidBody3DComponent>(entity, physSytem.CreateRigidBody(shipCollider));
				entityPhys.SetMass(1.f);
				entityPhys.SetAngularDamping(Nz::Vector3f::Zero());
				entityPhys.SetLinearDamping(0.f);
			}
		}
	}

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

	float elapsedTime = 0.f;
	Nz::UInt64 time = Nz::GetElapsedMicroseconds();

	Nz::PidController<Nz::Vector3f> headingController(0.5f, 0.f, 0.05f);
	Nz::PidController<Nz::Vector3f> upController(1.f, 0.f, 0.1f);

	bool showColliders = false;
	while (window.IsOpen())
	{
		Nz::UInt64 now = Nz::GetElapsedMicroseconds();
		elapsedTime = (now - time) / 1'000'000.f;
		time = now;

		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType::Quit:
					window.Close();
					break;

				case Nz::WindowEventType::KeyPressed:
					if (event.key.virtualKey == Nz::Keyboard::VKey::A)
						basicMat.EnableAlphaTest(!basicMat.IsAlphaTestEnabled());
					else if (event.key.virtualKey == Nz::Keyboard::VKey::B)
					{
						showColliders = !showColliders;
						if (showColliders)
						{
							auto view = registry.view<Nz::GraphicsComponent>();
							for (auto [entity, gfxComponent] : view.each())
								gfxComponent.AttachRenderable(colliderModel);
						}
						else
						{
							auto view = registry.view<Nz::GraphicsComponent>();
							for (auto [entity, gfxComponent] : view.each())
								gfxComponent.DetachRenderable(colliderModel);
						}
					}
					else if (event.key.virtualKey == Nz::Keyboard::VKey::Space)
					{
						entt::entity entity = registry.create();
						auto& entityGfx = registry.emplace<Nz::GraphicsComponent>(entity);
						entityGfx.AttachRenderable(model);
						if (showColliders)
							entityGfx.AttachRenderable(colliderModel);

						registry.emplace<Nz::NodeComponent>(entity);

						auto& entityPhys = registry.emplace<Nz::RigidBody3DComponent>(entity, physSytem.CreateRigidBody(shipCollider));
						entityPhys.SetMass(1.f);
						entityPhys.SetAngularDamping(Nz::Vector3f::Zero());
						entityPhys.SetLinearDamping(0.f);
					}

					break;

				case Nz::WindowEventType::MouseMoved:
				{
					float sensitivity = 0.3f;

					camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX * sensitivity;
					camAngles.pitch = camAngles.pitch - event.mouseMove.deltaY * sensitivity;

					camAngles.Normalize();

					camQuat = camAngles;

					registry.get<Nz::NodeComponent>(headingEntity).SetRotation(camQuat);
					break;
				}

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float updateTime = updateClock.Restart() / 1'000'000.f;

			float cameraSpeed = 20.f * updateTime;

			physSytem.Update(registry, 1000.f / 60.f);

			auto spaceshipView = registry.view<Nz::NodeComponent, Nz::RigidBody3DComponent>();
			for (auto&& [entity, node, _] : spaceshipView.each())
			{
				if (entity == playerEntity)
					continue;

				Nz::Vector3f spaceshipPos = node.GetPosition(Nz::CoordSys::Global);
				if (spaceshipPos.GetSquaredLength() > Nz::IntegralPow(200.f, 2))
					registry.destroy(entity);
			}

			Nz::RigidBody3DComponent& playerShipBody = registry.get<Nz::RigidBody3DComponent>(playerEntity);
			Nz::Quaternionf currentRotation = playerShipBody.GetRotation();

			Nz::Vector3f desiredHeading = registry.get<Nz::NodeComponent>(headingEntity).GetForward();
			Nz::Vector3f currentHeading = currentRotation * Nz::Vector3f::Forward();
			Nz::Vector3f headingError = currentHeading.CrossProduct(desiredHeading);

			Nz::Vector3f desiredUp = registry.get<Nz::NodeComponent>(headingEntity).GetUp();
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

		Nz::RenderFrame frame = windowImpl->Acquire();
		if (!frame)
			continue;

		renderSystem.Render(registry, frame);

		frame.Present();

		window.Display();

		fps++;

		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(registry.alive()) + " entities");

			fps = 0;

			secondClock.Restart();
		}
	}

	registry.clear();

	return EXIT_SUCCESS;
}
