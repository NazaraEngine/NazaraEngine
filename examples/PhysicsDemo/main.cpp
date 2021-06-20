#include <Nazara/Core.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/Components.hpp>
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

	Nz::ViewerInstance viewerInstance;
	viewerInstance.UpdateTargetSize(Nz::Vector2f(window.GetSize()));
	viewerInstance.UpdateProjViewMatrices(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f), Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1));

	Nz::VertexMapper vertexMapper(*spaceshipMesh->GetSubMesh(0), Nz::BufferAccess::ReadOnly);
	Nz::SparsePtr<Nz::Vector3f> vertices = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position);


	entt::registry registry;

	Nz::Physics3DSystem physSytem(registry);


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
	Nz::Node headingNode;
	{
		auto& entityNode = registry.emplace<Nz::NodeComponent>(playerEntity);
		entityNode.SetPosition(Nz::Vector3f(12.5f, 0.f, 25.f));

		auto& entityGfx = registry.emplace<Nz::GraphicsComponent>(playerEntity);
		entityGfx.AttachRenderable(model);

		auto& entityPhys = registry.emplace<Nz::RigidBody3DComponent>(playerEntity, physSytem.CreateRigidBody(shipCollider));
		entityPhys.SetMass(50.f);
		entityPhys.SetAngularDamping(Nz::Vector3f::Zero());

		headingNode.SetParent(entityNode);
		headingNode.SetInheritRotation(false);
		headingNode.SetRotation(entityNode.GetRotation());
	}


	Nz::Node cameraNode;
	cameraNode.SetParent(headingNode);
	cameraNode.SetPosition(Nz::Vector3f::Backward() * 2.5f + Nz::Vector3f::Up() * 1.f);

	for (std::size_t x = 0; x < 1; ++x)
	{
		for (std::size_t y = 0; y < 1; ++y)
		{
			for (std::size_t z = 0; z < 10; ++z)
			{
				entt::entity entity = registry.create();
				auto& entityNode = registry.emplace<Nz::NodeComponent>(entity);
				entityNode.SetPosition(Nz::Vector3f(x * 2.f, y * 1.5f, z * 2.f));

				auto& entityGfx = registry.emplace<Nz::GraphicsComponent>(entity);
				entityGfx.AttachRenderable(model);

				auto& entityPhys = registry.emplace<Nz::RigidBody3DComponent>(entity, physSytem.CreateRigidBody(shipCollider));
				entityPhys.SetMass(1.f);
				entityPhys.SetAngularDamping(Nz::Vector3f::Zero());
				entityPhys.SetLinearDamping(0.f);
			}
		}
	}


	Nz::RenderWindowImpl* windowImpl = window.GetImpl();
	std::shared_ptr<Nz::CommandPool> commandPool = windowImpl->CreateCommandPool(Nz::QueueType::Graphics);

	Nz::CommandBufferPtr drawCommandBuffer;
	auto RebuildCommandBuffer = [&]
	{
		Nz::Vector2ui windowSize = window.GetSize();
		drawCommandBuffer = commandPool->BuildCommandBuffer([&](Nz::CommandBufferBuilder& builder)
		{
			Nz::Recti renderRect(0, 0, window.GetSize().x, window.GetSize().y);

			Nz::CommandBufferBuilder::ClearValues clearValues[2];
			clearValues[0].color = Nz::Color(80, 80, 80);
			clearValues[1].depth = 1.f;
			clearValues[1].stencil = 0;

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green);
			{
				builder.BeginRenderPass(windowImpl->GetFramebuffer(), windowImpl->GetRenderPass(), renderRect, { clearValues[0], clearValues[1] });
				{
					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.BindShaderBinding(Nz::Graphics::ViewerBindingSet, viewerInstance.GetShaderBinding());

					auto view = registry.view<const Nz::GraphicsComponent>();
					for (auto [entity, gfxComponent] : view.each())
					{
						const Nz::WorldInstance& worldInstance = gfxComponent.GetWorldInstance();
						for (const auto& renderable : gfxComponent.GetRenderables())
							renderable->Draw(builder, worldInstance);
					}
				}
				builder.EndRenderPass();
			}
			builder.EndDebugRegion();
		});
	};


	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

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
	bool rebuildCommandBuffer = false;
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
					{
						basicMat.EnableAlphaTest(!basicMat.IsAlphaTestEnabled());
						rebuildCommandBuffer = true;
					}
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
						rebuildCommandBuffer = true;
					}

					break;

				case Nz::WindowEventType::MouseMoved:
				{
					float sensitivity = 0.3f;

					camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX * sensitivity;
					camAngles.pitch = camAngles.pitch - event.mouseMove.deltaY * sensitivity;

					camAngles.Normalize();

					camQuat = camAngles;

					headingNode.SetRotation(camQuat);
					break;
				}

				case Nz::WindowEventType::Resized:
				{
					Nz::Vector2ui windowSize = window.GetSize();
					viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f));
					viewerInstance.UpdateTargetSize(Nz::Vector2f(windowSize));
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

			Nz::RigidBody3DComponent& playerShipBody = registry.get<Nz::RigidBody3DComponent>(playerEntity);
			Nz::Quaternionf currentRotation = playerShipBody.GetRotation();

			Nz::Vector3f desiredHeading = headingNode.GetForward();
			Nz::Vector3f currentHeading = currentRotation * Nz::Vector3f::Forward();
			Nz::Vector3f headingError = currentHeading.CrossProduct(desiredHeading);

			Nz::Vector3f desiredUp = headingNode.GetUp();
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

		Nz::UploadPool& uploadPool = frame.GetUploadPool();

		viewerInstance.UpdateViewMatrix(Nz::Matrix4f::ViewMatrix(cameraNode.GetPosition(Nz::CoordSys::Global), cameraNode.GetRotation(Nz::CoordSys::Global)));

		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("UBO Update", Nz::Color::Yellow);
			{
				builder.PreTransferBarrier();

				viewerInstance.UpdateBuffers(uploadPool, builder);
				/*
				modelInstance.UpdateBuffers(uploadPool, builder);
				modelInstance2.UpdateBuffers(uploadPool, builder);
				*/

				auto view = registry.view<Nz::GraphicsComponent, const Nz::NodeComponent>();
				for (auto [entity, gfxComponent, nodeComponent] : view.each())
				{
					Nz::WorldInstance& worldInstance = gfxComponent.GetWorldInstance();
					worldInstance.UpdateWorldMatrix(nodeComponent.GetTransformMatrix());

					worldInstance.UpdateBuffers(uploadPool, builder);
				}

				if (material->Update(frame, builder))
					rebuildCommandBuffer = true;

				if (colliderMat->Update(frame, builder))
					rebuildCommandBuffer = true;

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, Nz::QueueType::Transfer);

		if (rebuildCommandBuffer || frame.IsFramebufferInvalidated())
		{
			frame.PushForRelease(std::move(drawCommandBuffer));
			RebuildCommandBuffer();
		}

		frame.SubmitCommandBuffer(drawCommandBuffer.get(), Nz::QueueType::Graphics);

		frame.Present();

		window.Display();

		rebuildCommandBuffer = false;

		fps++;

		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");

			fps = 0;

			secondClock.Restart();
		}
	}

	registry.clear();

	return EXIT_SUCCESS;
}
