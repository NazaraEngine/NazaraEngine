#include <Nazara/Core.hpp>
#include <Nazara/Core/Systems.hpp>
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
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <Nazara/Utility/Systems.hpp>
#include <Nazara/Utility/Plugins/AssimpPlugin.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <iostream>
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Graphics> nazara(rendererConfig);

	Nz::PluginLoader loader;
	Nz::Plugin<Nz::AssimpPlugin> assimp = loader.Load<Nz::AssimpPlugin>();

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	entt::registry registry;
	Nz::SystemGraph systemGraph(registry);
	systemGraph.AddSystem<Nz::SkeletonSystem>();

	Nz::Physics3DSystem& physSytem = systemGraph.AddSystem<Nz::Physics3DSystem>();
	Nz::RenderSystem& renderSystem = systemGraph.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Skinning test";
	Nz::RenderWindow& window = renderSystem.CreateWindow(device, Nz::VideoMode(1280, 720, 32), windowTitle);

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

	Nz::MeshParams meshParams;
	meshParams.animated = true;
	meshParams.center = true;
	meshParams.vertexScale = Nz::Vector3f(0.1f, 0.1f, 0.1f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent_Skinning);

	std::shared_ptr<Nz::Mesh> bobMesh = Nz::Mesh::LoadFromFile(resourceDir / "character/Gangnam Style.fbx", meshParams);
	if (!bobMesh)
	{
		NazaraError("Failed to load bob mesh");
		return __LINE__;
	}

	Nz::AnimationParams animParam;
	animParam.skeleton = bobMesh->GetSkeleton();
	animParam.jointRotation = meshParams.vertexRotation;
	animParam.jointScale = meshParams.vertexScale;
	animParam.jointOffset = meshParams.vertexOffset;

	std::shared_ptr<Nz::Animation> bobAnim = Nz::Animation::LoadFromFile(resourceDir / "character/Gangnam Style.fbx", animParam);
	if (!bobAnim)
	{
		NazaraError("Failed to load bob anim");
		return __LINE__;
	}

	std::shared_ptr<Nz::Skeleton> skeleton = std::make_shared<Nz::Skeleton>(*bobMesh->GetSkeleton());

	std::cout << "joint count: " << skeleton->GetJointCount() << std::endl;

	const Nz::Boxf& bobAABB = bobMesh->GetAABB();
	std::shared_ptr<Nz::GraphicalMesh> bobGfxMesh = Nz::GraphicalMesh::BuildFromMesh(*bobMesh);

	std::shared_ptr<Nz::Material> material = Nz::Graphics::Instance()->GetDefaultMaterials().basicMaterial;

	std::shared_ptr<Nz::Model> bobModel = std::make_shared<Nz::Model>(std::move(bobGfxMesh), bobAABB);
	std::vector<std::shared_ptr<Nz::MaterialInstance>> materials(bobMesh->GetMaterialCount());

	for (std::size_t i = 0; i < bobMesh->GetMaterialCount(); ++i)
	{
		materials[i] = std::make_shared<Nz::MaterialInstance>(material);

		std::string matPath = bobMesh->GetMaterialData(i).GetStringParameter(Nz::MaterialData::BaseColorTexturePath).GetValueOr("");
		if (!matPath.empty())
			materials[i]->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(matPath, texParams));
	}

	for (std::size_t i = 0; i < bobMesh->GetSubMeshCount(); ++i)
	{
		std::size_t matIndex = bobMesh->GetSubMesh(i)->GetMaterialIndex();
		if (materials[matIndex])
			bobModel->SetMaterial(i, materials[matIndex]);
	}

	/*for (std::size_t y = 0; y < 10; ++y)
	{
		for (std::size_t x = 0; x < 10; ++x)
		{
			entt::entity bobEntity = registry.create();

			auto& bobNode = registry.emplace<Nz::NodeComponent>(bobEntity);
			bobNode.SetPosition(Nz::Vector3f(x - 5.f, 0.f, -float(y)));
			//bobNode.SetRotation(Nz::EulerAnglesf(-90.f, -90.f, 0.f));
			//bobNode.SetScale(1.f / 40.f * 0.5f);

			auto& bobGfx = registry.emplace<Nz::GraphicsComponent>(bobEntity);
			bobGfx.AttachRenderable(bobModel, 0xFFFFFFFF);
		}
	}*/

	entt::handle bobEntity = entt::handle(registry, registry.create());
	{
		auto& bobNode = bobEntity.emplace<Nz::NodeComponent>();
		//bobNode.SetRotation(Nz::EulerAnglesf(-90.f, -90.f, 0.f));
		//bobNode.SetScale(1.f / 40.f * 0.5f);
		//bobNode.SetPosition(Nz::Vector3f(0.f, -1.f, 0.f));

		auto& bobGfx = bobEntity.emplace<Nz::GraphicsComponent>();
		bobGfx.AttachRenderable(bobModel, 0xFFFFFFFF);

		auto& sharedSkeleton = bobEntity.emplace<Nz::SharedSkeletonComponent>(skeleton);


		entt::entity sphereEntity = registry.create();
		{
			std::shared_ptr<Nz::Mesh> sphereMesh = std::make_shared<Nz::Mesh>();
			sphereMesh->CreateStatic();
			sphereMesh->BuildSubMesh(Nz::Primitive::UVSphere(1.f, 50, 50));
			sphereMesh->SetMaterialCount(1);
			sphereMesh->GenerateNormalsAndTangents();

			std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*sphereMesh);

			// Textures
			Nz::TextureParams texParams;
			texParams.renderDevice = device;

			Nz::TextureParams srgbTexParams = texParams;
			srgbTexParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

			std::shared_ptr<Nz::MaterialInstance> sphereMat = std::make_shared<Nz::MaterialInstance>(material);
			sphereMat->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "Rusty/rustediron2_basecolor.png", srgbTexParams));

			std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(std::move(gfxMesh), sphereMesh->GetAABB());
			for (std::size_t i = 0; i < sphereModel->GetSubMeshCount(); ++i)
				sphereModel->SetMaterial(i, sphereMat);

			auto& sphereNode = registry.emplace<Nz::NodeComponent>(sphereEntity);
			sphereNode.SetScale(0.1f);
			sphereNode.SetInheritScale(false);
			sphereNode.SetParentJoint(bobEntity, "RightHand");

			auto& sphereBody = registry.emplace<Nz::RigidBody3DComponent>(sphereEntity, &physSytem.GetPhysWorld());
			sphereBody.SetGeom(std::make_shared<Nz::SphereCollider3D>(0.1f));

			auto& sphereGfx = registry.emplace<Nz::GraphicsComponent>(sphereEntity);
			sphereGfx.AttachRenderable(sphereModel, 0xFFFFFFFF);
		}

		entt::entity smallBobEntity = registry.create();
		auto& smallBobNode = registry.emplace<Nz::NodeComponent>(smallBobEntity);
		smallBobNode.SetParentJoint(bobEntity, "LeftHand");

		auto& smallBobGfx = registry.emplace<Nz::GraphicsComponent>(smallBobEntity);
		smallBobGfx.AttachRenderable(bobModel, 0xFFFFFFFF);

		registry.emplace<Nz::SharedSkeletonComponent>(smallBobEntity, skeleton);
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

		std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = Nz::GraphicalMesh::BuildFromMesh(planeMesh);

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

		std::shared_ptr<Nz::MaterialInstance> planeMat = std::make_shared<Nz::MaterialInstance>(material);
		planeMat->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "dev_grey.png", texParams), planeSampler);

		std::shared_ptr<Nz::Model> planeModel = std::make_shared<Nz::Model>(std::move(planeMeshGfx), planeMesh.GetAABB());
		planeModel->SetMaterial(0, planeMat);

		auto& planeGfx = registry.emplace<Nz::GraphicsComponent>(planeEntity);
		planeGfx.AttachRenderable(planeModel, 0xFFFFFFFF);

		auto& planeNode = registry.emplace<Nz::NodeComponent>(planeEntity);

		auto& planeBody = registry.emplace<Nz::RigidBody3DComponent>(planeEntity, &physSytem.GetPhysWorld());
		planeBody.SetGeom(std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(planeSize.x, 0.5f, planeSize.y), Nz::Vector3f(0.f, -0.25f, 0.f)));
	}

	window.EnableEventPolling(true);

	Nz::Clock fpsClock, updateClock;
	float incr = 0.f;
	unsigned int currentFrame = 0;
	unsigned int nextFrame = 1;
	Nz::EulerAnglesf camAngles = Nz::EulerAnglesf(-30.f, 0.f, 0.f);
	Nz::UInt64 lastTime = Nz::GetElapsedMicroseconds();
	Nz::UInt64 fps = 0;
	bool paused = false;

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
				{
					if (event.type == Nz::WindowEventType::KeyPressed && event.key.virtualKey == Nz::Keyboard::VKey::P)
						paused = !paused;

					break;
				}

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

			if (!paused)
			{
				incr += updateTime * bobAnim->GetSequence(0)->frameRate * 1.5f;
				while (incr >= 1.f)
				{
					incr -= 1.f;

					currentFrame = nextFrame;
					nextFrame++;
					if (nextFrame >= bobAnim->GetFrameCount())
						nextFrame = 0;
				}

				std::cout << currentFrame << std::endl;

				bobAnim->AnimateSkeleton(skeleton.get(), currentFrame, nextFrame, incr);
			}
			//for (std::size_t i = 0; i < skeleton.GetJointCount(); ++i)
			//	matrices[i] = skeleton.GetJoint(i)->GetSkinningMatrix();

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

		Nz::DebugDrawer& debugDrawer = renderSystem.GetFramePipeline().GetDebugDrawer();
		//debugDrawer.DrawSkeleton(*skeleton, Nz::Color::Red);

		systemGraph.Update();

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
