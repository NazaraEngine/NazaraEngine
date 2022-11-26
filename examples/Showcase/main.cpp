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
#include <bitset>
#include <iostream>
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
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
		//cameraNode.SetParent(playerRotNode);

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

	//std::shared_ptr<Nz::Material> material = Nz::Graphics::Instance()->GetDefaultMaterials().basicTransparent;

	std::shared_ptr<Nz::Model> bobModel = std::make_shared<Nz::Model>(std::move(bobGfxMesh), bobAABB);
	std::vector<std::shared_ptr<Nz::MaterialInstance>> materials(bobMesh->GetMaterialCount());

	std::bitset<5> alphaMaterials("01010");
	for (std::size_t i = 0; i < bobMesh->GetMaterialCount(); ++i)
	{
		const Nz::ParameterList& materialData = bobMesh->GetMaterialData(i);
		std::string matPath = materialData.GetStringParameter(Nz::MaterialData::BaseColorTexturePath).GetValueOr("");
		if (!matPath.empty())
		{
			Nz::MaterialInstanceParams params;
			params.lightingType = Nz::MaterialLightingType::Phong;
			if (alphaMaterials.test(i))
				params.custom.SetParameter("EnableAlphaBlending", true);

			materials[i] = Nz::MaterialInstance::LoadFromFile(matPath, params);
		}
		else
			materials[i] = Nz::Graphics::Instance()->GetDefaultMaterials().basicDefault;
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
	entt::entity lightEntity1 = registry.create();
	{
		auto& lightNode = registry.emplace<Nz::NodeComponent>(lightEntity1);
		lightNode.SetPosition(Nz::Vector3f::Up() * 3.f + Nz::Vector3f::Backward() * 1.f);
		lightNode.SetRotation(Nz::EulerAnglesf(-70.f, 0.f, 0.f));

		auto& cameraLight = registry.emplace<Nz::LightComponent>(lightEntity1);

		auto& spotLight = cameraLight.AddLight<Nz::SpotLight>(0xFFFFFFFF);
		spotLight.UpdateColor(Nz::Color::Red);
		spotLight.UpdateInnerAngle(Nz::DegreeAnglef(15.f));
		spotLight.UpdateOuterAngle(Nz::DegreeAnglef(20.f));
		spotLight.EnableShadowCasting(true);
	}

	entt::entity lightEntity2 = registry.create();
	{
		auto& lightNode = registry.emplace<Nz::NodeComponent>(lightEntity2);
		lightNode.SetPosition(Nz::Vector3f::Up() * 3.5f + Nz::Vector3f::Right() * 1.5f);
		lightNode.SetRotation(Nz::EulerAnglesf(-70.f, 90.f, 0.f));

		auto& cameraLight = registry.emplace<Nz::LightComponent>(lightEntity2);

		auto& spotLight = cameraLight.AddLight<Nz::SpotLight>(0xFFFFFFFF);
		spotLight.UpdateColor(Nz::Color::Green);
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateShadowMapSize(1024);
	}
	entt::entity lightEntity3 = registry.create();

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
			Nz::TextureParams srgbTexParams = texParams;
			srgbTexParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

			std::shared_ptr<Nz::MaterialInstance> sphereMat = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
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

		{
			auto& lightNode = registry.emplace<Nz::NodeComponent>(lightEntity3);
			lightNode.SetPosition(Nz::Vector3f::Backward() * 2.f);
			lightNode.SetRotation(Nz::EulerAnglesf(-45.f, 180.f, 0.f));
			lightNode.SetParentJoint(bobEntity, "Spine2");

			auto& cameraLight = registry.emplace<Nz::LightComponent>(lightEntity3);

			auto& spotLight = cameraLight.AddLight<Nz::SpotLight>(0xFFFFFFFF);
			spotLight.UpdateColor(Nz::Color::Blue);
			spotLight.UpdateInnerAngle(Nz::DegreeAnglef(15.f));
			spotLight.UpdateOuterAngle(Nz::DegreeAnglef(20.f));
			spotLight.EnableShadowCasting(true);
		}
	}

	std::shared_ptr<Nz::MaterialInstance> textMat = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
	for (const char* pass : { "DepthPass", "ShadowPass", "ForwardPass" })
	{
		textMat->UpdatePassStates(pass, [](Nz::RenderStates& renderStates)
		{
			renderStates.faceCulling = Nz::FaceCulling::None;
			return true;
		});
	}

	textMat->UpdatePassStates("ForwardPass", [](Nz::RenderStates& renderStates)
	{
		renderStates.depthWrite = false;
		renderStates.blending = true;
		renderStates.blend.modeColor = Nz::BlendEquation::Add;
		renderStates.blend.modeAlpha = Nz::BlendEquation::Add;
		renderStates.blend.srcColor = Nz::BlendFunc::SrcAlpha;
		renderStates.blend.dstColor = Nz::BlendFunc::InvSrcAlpha;
		renderStates.blend.srcAlpha = Nz::BlendFunc::One;
		renderStates.blend.dstAlpha = Nz::BlendFunc::One;
		return true;
	});

	textMat->SetValueProperty("AlphaTest", true);

	std::shared_ptr<Nz::TextSprite> sprite = std::make_shared<Nz::TextSprite>(textMat);
	sprite->UpdateRenderLayer(1);
	sprite->Update(Nz::SimpleTextDrawer::Draw("Shadow-mapping !", 72), 0.002f);

	entt::entity textEntity = registry.create();
	{
		auto& entityGfx = registry.emplace<Nz::GraphicsComponent>(textEntity);
		entityGfx.AttachRenderable(sprite, 1);

		auto& entityNode = registry.emplace<Nz::NodeComponent>(textEntity);
		entityNode.SetPosition(Nz::Vector3f::Up() * 0.5f + Nz::Vector3f::Backward() * 0.66f + Nz::Vector3f::Left() * 0.5f);
		entityNode.SetRotation(Nz::EulerAnglesf(-45.f, 0.f, 0.f));
	}

	entt::entity planeEntity = registry.create();
	Nz::Boxf floorBox;
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

		std::shared_ptr<Nz::MaterialInstance> planeMat = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
		planeMat->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "dev_grey.png", texParams), planeSampler);

		floorBox = planeMesh.GetAABB();

		std::shared_ptr<Nz::Model> planeModel = std::make_shared<Nz::Model>(std::move(planeMeshGfx), planeMesh.GetAABB());
		planeModel->SetMaterial(0, planeMat);

		auto& planeGfx = registry.emplace<Nz::GraphicsComponent>(planeEntity);
		planeGfx.AttachRenderable(planeModel, 0xFFFFFFFF);

		registry.emplace<Nz::NodeComponent>(planeEntity);

		auto& planeBody = registry.emplace<Nz::RigidBody3DComponent>(planeEntity, &physSytem.GetPhysWorld());
		planeBody.SetGeom(std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(planeSize.x, 0.5f, planeSize.y), Nz::Vector3f(0.f, -0.25f, 0.f)));

		Nz::Mesh boxMesh;
		boxMesh.CreateStatic();
		boxMesh.BuildSubMesh(Nz::Primitive::Box(Nz::Vector3f(0.5f, 0.5f, 0.5f)), meshPrimitiveParams);
		boxMesh.SetMaterialCount(1);

		std::shared_ptr<Nz::GraphicalMesh> boxMeshGfx = Nz::GraphicalMesh::BuildFromMesh(boxMesh);

		std::shared_ptr<Nz::Model> boxModel = std::make_shared<Nz::Model>(std::move(boxMeshGfx), boxMesh.GetAABB());
		boxModel->SetMaterial(0, planeMat);

		entt::entity boxEntity = registry.create();
		registry.emplace<Nz::NodeComponent>(boxEntity).SetPosition(Nz::Vector3f(0.f, 0.25f, -0.5f));
		registry.emplace<Nz::GraphicsComponent>(boxEntity).AttachRenderable(boxModel, 0xFFFFFFFF);
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

					/*auto& playerRotNode = registry.get<Nz::NodeComponent>(playerRotation);
					playerRotNode.SetRotation(camAngles);*/
					auto& playerRotNode = registry.get<Nz::NodeComponent>(playerCamera);
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

			/*auto& playerBody = registry.get<Nz::RigidBody3DComponent>(playerEntity);

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
				playerBody.AddForce(Nz::Vector3f::Right() * 25.f * mass, Nz::CoordSys::Local);*/

			float cameraSpeed = 2.f;

			auto& cameraNode = registry.get<Nz::NodeComponent>(playerCamera);
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Space))
				cameraNode.Move(Nz::Vector3f::Up() * cameraSpeed * updateTime, Nz::CoordSys::Global);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				cameraNode.Move(Nz::Vector3f::Forward() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				cameraNode.Move(Nz::Vector3f::Backward() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				cameraNode.Move(Nz::Vector3f::Left() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				cameraNode.Move(Nz::Vector3f::Right() * cameraSpeed * updateTime, Nz::CoordSys::Local);

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
		auto& lightNode = registry.get<Nz::NodeComponent>(lightEntity3);
		//debugDrawer.DrawLine(lightNode.GetPosition(Nz::CoordSys::Global), lightNode.GetForward() * 10.f, Nz::Color::Blue);
		/*Nz::Boxf test = spotLight->GetBoundingVolume().aabb;
		debugDrawer.DrawBox(test, Nz::Color::Blue);
		debugDrawer.DrawBox(floorBox, Nz::Color::Red);
		Nz::Boxf intersection;
		if (floorBox.Intersect(test, &intersection))
			debugDrawer.DrawBox(intersection, Nz::Color::Green);*/

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
