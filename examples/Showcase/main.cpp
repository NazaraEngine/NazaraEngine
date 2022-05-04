#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
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
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "resources";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
		resourceDir = ".." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	//rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Graphics> nazara(rendererConfig);

	Nz::PluginManager::Mount(Nz::Plugin::Assimp);
	Nz::CallOnExit unmountAssimp([]
	{
		Nz::PluginManager::Unmount(Nz::Plugin::Assimp);
	});

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	std::string windowTitle = "Skinning test";
	Nz::RenderWindow window;
	if (!window.Create(device, Nz::VideoMode(1280, 720, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		return __LINE__;
	}

	entt::registry registry;

	Nz::Physics3DSystem physSytem(registry);
	Nz::RenderSystem renderSystem(registry);

	physSytem.GetPhysWorld().SetGravity({ 0.f, -9.81f, 0.f });

	Nz::TextureParams texParams;
	texParams.renderDevice = device;

	entt::entity playerEntity = registry.create();
	entt::entity playerRotation = registry.create();
	entt::entity playerCamera = registry.create();
	{
		auto& playerNode = registry.emplace<Nz::NodeComponent>(playerEntity);
		playerNode.SetPosition(0.f, 1.8f, 1.f);

		auto& playerBody = registry.emplace<Nz::RigidBody3DComponent>(playerEntity, &physSytem.GetPhysWorld());
		playerBody.SetMass(42.f);
		playerBody.SetGeom(std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f::Unit()));

		auto& playerRotNode = registry.emplace<Nz::NodeComponent>(playerRotation);
		playerRotNode.SetParent(playerNode);

		auto& cameraNode = registry.emplace<Nz::NodeComponent>(playerCamera);
		cameraNode.SetParent(playerRotNode);

		auto& cameraComponent = registry.emplace<Nz::CameraComponent>(playerCamera, window.GetRenderTarget());
		cameraComponent.UpdateZNear(0.2f);
		cameraComponent.UpdateZFar(10000.f);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	Nz::FieldOffsets skeletalOffsets(Nz::StructLayout::Std140);
	std::size_t arrayOffset = skeletalOffsets.AddMatrixArray(Nz::StructFieldType::Float1, 4, 4, true, 200);

	std::vector<Nz::UInt8> skeletalBufferMem(skeletalOffsets.GetAlignedSize());
	Nz::Matrix4f* matrices = Nz::AccessByOffset<Nz::Matrix4f*>(skeletalBufferMem.data(), arrayOffset);

	Nz::MeshParams meshParams;
	meshParams.animated = true;
	meshParams.center = true;
	meshParams.texCoordScale = Nz::Vector2f(1.f, -1.f);
	meshParams.texCoordOffset = Nz::Vector2f(0.f, 1.f);
	//meshParams.matrix = Nz::Matrix4f::Scale(Nz::Vector3f(10.f));
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent_Skinning);

	std::shared_ptr<Nz::Mesh> bobMesh = Nz::Mesh::LoadFromFile(resourceDir / "hellknight/hellknight.md5mesh", meshParams);
	if (!bobMesh)
	{
		NazaraError("Failed to load bob mesh");
		return __LINE__;
	}

	Nz::AnimationParams matParams;
	matParams.skeleton = bobMesh->GetSkeleton();

	std::shared_ptr<Nz::Animation> bobAnim = Nz::Animation::LoadFromFile(resourceDir / "hellknight/idle.md5anim", matParams);
	if (!bobAnim)
	{
		NazaraError("Failed to load bob anim");
		return __LINE__;
	}

	Nz::Skeleton skeleton = *bobMesh->GetSkeleton();

	std::cout << "joint count: " << skeleton.GetJointCount() << std::endl;
	//std::cout << "anim joint count: " << bobAnim->GetJointCount() << std::endl;

	//bobAnim->AnimateSkeleton(&skeleton, 0, 1, 0.5f);

	/*for (std::size_t i = 0; i < bobMesh->GetSubMeshCount(); ++i)
	{
		Nz::VertexMapper mapper(*bobMesh->GetSubMesh(i));

		Nz::SkinningData skinningData;
		skinningData.joints = skeleton.GetJoints();
		skinningData.inputJointIndices = mapper.GetComponentPtr<Nz::Vector4i32>(Nz::VertexComponent::JointIndices);
		skinningData.inputJointWeights = mapper.GetComponentPtr<Nz::Vector4f>(Nz::VertexComponent::JointWeights);
		skinningData.outputPositions = mapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position);
		skinningData.inputPositions = skinningData.outputPositions;

		Nz::SkinPosition(skinningData, 0, mapper.GetVertexCount());
	}*/

	/*for (std::size_t i = 0; i < skeleton.GetJointCount(); ++i)
		matrices[i] = skeleton.GetJoint(i)->GetSkinningMatrix();*/

	std::shared_ptr<Nz::RenderBuffer> renderBuffer = device->InstantiateBuffer(Nz::BufferType::Uniform, skeletalBufferMem.size(), Nz::BufferUsage::Write, skeletalBufferMem.data());

	const Nz::Boxf& bobAABB = bobMesh->GetAABB();
	std::shared_ptr<Nz::GraphicalMesh> bobGfxMesh = std::make_shared<Nz::GraphicalMesh>(*bobMesh);

	std::shared_ptr<Nz::Model> bobModel = std::make_shared<Nz::Model>(std::move(bobGfxMesh), bobAABB);
	for (std::size_t i = 0; i < bobMesh->GetMaterialCount(); ++i)
	{
		std::string matPath;
		bobMesh->GetMaterialData(i).GetStringParameter(Nz::MaterialData::DiffuseTexturePath, &matPath);

		std::shared_ptr<Nz::Material> bobMat = std::make_shared<Nz::Material>();

		std::shared_ptr<Nz::MaterialPass> bobMatPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
		bobMatPass->SetSharedUniformBuffer(0, renderBuffer);

		bobMatPass->EnableDepthBuffer(true);
		{
			Nz::BasicMaterial basicMat(*bobMatPass);
			if (matPath.find("gob") != matPath.npos)
			{
				bobMatPass->EnableFlag(Nz::MaterialPassFlag::SortByDistance);

				basicMat.SetAlphaMap(Nz::Texture::LoadFromFile(matPath, texParams));
				bobMatPass->EnableDepthWrite(false);

				bobMatPass->EnableBlending(true);
				bobMatPass->SetBlendEquation(Nz::BlendEquation::Add, Nz::BlendEquation::Add);
				bobMatPass->SetBlendFunc(Nz::BlendFunc::SrcAlpha, Nz::BlendFunc::InvSrcAlpha, Nz::BlendFunc::One, Nz::BlendFunc::Zero);
			}
			else
				basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(matPath, texParams));
		}

		bobMat->AddPass("ForwardPass", bobMatPass);

		bobModel->SetMaterial(i, bobMat);
	}

	/*for (std::size_t y = 0; y < 50; ++y)
	{
		for (std::size_t x = 0; x < 50; ++x)
		{
			entt::entity bobEntity = registry.create();

			auto& bobNode = registry.emplace<Nz::NodeComponent>(bobEntity);
			bobNode.SetPosition(Nz::Vector3f(x - 25.f, bobAABB.height / 2.f, -float(y)));
			bobNode.SetRotation(Nz::EulerAnglesf(-90.f, -90.f, 0.f));
			bobNode.SetScale(1.f / 40.f * 0.5f);

			auto& bobGfx = registry.emplace<Nz::GraphicsComponent>(bobEntity);
			bobGfx.AttachRenderable(bobModel, 0xFFFFFFFF);
		}
	}*/

	entt::entity bobEntity = registry.create();
	{
		auto& bobNode = registry.emplace<Nz::NodeComponent>(bobEntity);
		bobNode.SetRotation(Nz::EulerAnglesf(90.f, -90.f, 0.f));
		bobNode.SetScale(1.f / 40.f * 0.5f);
		bobNode.SetPosition(bobNode.GetScale() * Nz::Vector3f(0.f, bobAABB.height / 2.f, 0.f));

		auto& bobGfx = registry.emplace<Nz::GraphicsComponent>(bobEntity);
		bobGfx.AttachRenderable(bobModel, 0xFFFFFFFF);
	}

	entt::entity planeEntity = registry.create();
	{
		Nz::MeshParams meshPrimitiveParams;
		meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

		Nz::Vector2f planeSize(25.f, 25.f);

		Nz::Mesh planeMesh;
		planeMesh.CreateStatic();
		planeMesh.BuildSubMesh(Nz::Primitive::Plane(planeSize, Nz::Vector2ui(0u), Nz::Matrix4f::Identity(), Nz::Rectf(0.f, 0.f, 10.f, 10.f)), meshPrimitiveParams);
		planeMesh.SetMaterialCount(1);

		std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = std::make_shared<Nz::GraphicalMesh>(planeMesh);

		std::shared_ptr<Nz::Material> planeMat = std::make_shared<Nz::Material>();

		std::shared_ptr<Nz::MaterialPass> planeMatPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
		planeMatPass->EnableDepthBuffer(true);
		{
			Nz::BasicMaterial basicMat(*planeMatPass);
			basicMat.SetDiffuseMap(Nz::Texture::LoadFromFile(resourceDir / "dev_grey.png", texParams));

			Nz::TextureSamplerInfo planeSampler;
			planeSampler.anisotropyLevel = 16;
			planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
			planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;
			basicMat.SetDiffuseSampler(planeSampler);
		}
		planeMat->AddPass("ForwardPass", planeMatPass);

		std::shared_ptr<Nz::Model> planeModel = std::make_shared<Nz::Model>(std::move(planeMeshGfx), planeMesh.GetAABB());
		planeModel->SetMaterial(0, planeMat);

		auto& planeNode = registry.emplace<Nz::NodeComponent>(planeEntity);

		auto& planeBody = registry.emplace<Nz::RigidBody3DComponent>(planeEntity, &physSytem.GetPhysWorld());
		planeBody.SetGeom(std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(planeSize.x, 0.5f, planeSize.y), Nz::Vector3f(0.f, -0.25f, 0.f)));

		auto& planeGfx = registry.emplace<Nz::GraphicsComponent>(planeEntity);
		planeGfx.AttachRenderable(planeModel, 0xFFFFFFFF);
	}

	window.EnableEventPolling(true);

	Nz::Clock fpsClock, updateClock;
	float incr = 0.f;
	unsigned int currentFrame = 0;
	unsigned int nextFrame = 1;
	Nz::EulerAnglesf camAngles = Nz::EulerAnglesf(-30.f, 0.f, 0.f);
	Nz::UInt64 lastTime = Nz::GetElapsedMicroseconds();
	Nz::UInt64 fps = 0;
	while (window.IsOpen())
	{
		Nz::UInt64 now = Nz::GetElapsedMicroseconds();
		Nz::UInt64 elapsedTime = (now - lastTime) / 1'000'000.f;
		lastTime = now;

		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType::Quit:
					window.Close();
					break;

				case Nz::WindowEventType::KeyPressed:
					break;

				case Nz::WindowEventType::MouseMoved:
				{
					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX * sensitivity;
					camAngles.yaw.Normalize();

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch - event.mouseMove.deltaY * sensitivity, -89.f, 89.f);

					auto& playerRotNode = registry.get<Nz::NodeComponent>(playerRotation);
					playerRotNode.SetRotation(camAngles);
					break;
				}

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float updateTime = updateClock.Restart() / 1'000'000.f;

			physSytem.Update(registry, 1.f / 60.f);

			auto& playerBody = registry.get<Nz::RigidBody3DComponent>(playerEntity);

			float mass = playerBody.GetMass();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Space))
				playerBody.AddForce(Nz::Vector3f(0.f, mass * 50.f, 0.f));

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				playerBody.AddForce(Nz::Vector3f::Forward() * 25.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				playerBody.AddForce(Nz::Vector3f::Backward() * 25.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				playerBody.AddForce(Nz::Vector3f::Left() * 25.f * mass, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				playerBody.AddForce(Nz::Vector3f::Right() * 25.f * mass, Nz::CoordSys::Local);

			incr += 1.f / 60.f * 30.f;
			if (incr >= 1.f)
			{
				incr -= 1.f;

				currentFrame = nextFrame;
				nextFrame++;
				if (nextFrame >= bobAnim->GetFrameCount())
					nextFrame = 0;
			}

			std::cout << currentFrame << std::endl;

			bobAnim->AnimateSkeleton(&skeleton, currentFrame, nextFrame, incr);
			for (std::size_t i = 0; i < skeleton.GetJointCount(); ++i)
				matrices[i] = skeleton.GetJoint(i)->GetSkinningMatrix();

			//renderBuffer->Fill(skeletalBufferMem.data(), 0, skeletalBufferMem.size());

			/*auto spaceshipView = registry.view<Nz::NodeComponent, Nz::RigidBody3DComponent>();
			for (auto&& [entity, node, _] : spaceshipView.each())
			{
				if (entity == playerEntity)
					continue;

				Nz::Vector3f spaceshipPos = node.GetPosition(Nz::CoordSys::Global);
				if (spaceshipPos.GetSquaredLength() > Nz::IntegralPow(20.f, 2))
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
				playerShipBody.AddForce(Nz::Vector3f::Down() * 3.f * mass, Nz::CoordSys::Local);*/
		}

		Nz::RenderFrame frame = window.AcquireFrame();
		if (!frame)
			continue;

		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("Skeletal UBO Update", Nz::Color::Yellow);
			{
				builder.PreTransferBarrier();

				auto& skeletalAllocation = frame.GetUploadPool().Allocate(skeletalBufferMem.size());
				std::memcpy(skeletalAllocation.mappedPtr, skeletalBufferMem.data(), skeletalBufferMem.size());

				builder.CopyBuffer(skeletalAllocation, Nz::RenderBufferView(renderBuffer.get()));

				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();
		}, Nz::QueueType::Graphics);

		renderSystem.Render(registry, frame);

		frame.Present();

		fps++;

		if (fpsClock.GetMilliseconds() >= 1000)
		{
			fpsClock.Restart();

			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(registry.alive()) + " entities");

			fps = 0;
		}
	}

	return EXIT_SUCCESS;
}
