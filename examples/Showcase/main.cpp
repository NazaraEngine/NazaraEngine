#include <Nazara/Core.hpp>
#include <Nazara/Core/Plugins/AssimpPlugin.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/TextRenderer/SimpleTextDrawer.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <bitset>
#include <iostream>
#include <limits>

NAZARA_REQUEST_DEDICATED_GPU()

int main(int argc, char* argv[])
{
	Nz::Application<Nz::Graphics, Nz::Physics3D> app(argc, argv);

	Nz::PluginLoader loader;
	Nz::Plugin<Nz::AssimpPlugin> assimp = loader.Load<Nz::AssimpPlugin>();

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	world.AddSystem<Nz::SkeletonSystem>();

	Nz::Physics3DSystem& physSytem = world.AddSystem<Nz::Physics3DSystem>();
	physSytem.GetPhysWorld().SetGravity(Nz::Vector3f::Zero());
	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	std::string windowTitle = "Skinning test";
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), windowTitle);
	auto& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	auto& fs = app.AddComponent<Nz::FilesystemAppComponent>();
	{
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
	}

	physSytem.GetPhysWorld().SetGravity({ 0.f, -9.81f, 0.f });

	std::optional<Nz::PhysCharacter3D> character;

	entt::handle playerEntity = world.CreateEntity();
	entt::handle playerRotation = world.CreateEntity();
	entt::handle playerCamera = world.CreateEntity();
	{
		auto& playerNode = playerEntity.emplace<Nz::NodeComponent>();
		playerNode.SetPosition(0.f, 1.8f, 1.f);

		auto playerCollider = std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(0.2f, 1.8f, 0.2f));

		//auto& playerBody = playerEntity.emplace<Nz::RigidBody3DComponent>(physSytem.CreateRigidBody(playerCollider));
		//playerBody.SetMass(42.f);

		Nz::PhysCharacter3D::Settings characterSettings;
		characterSettings.collider = playerCollider;
		characterSettings.position = Nz::Vector3f::Up() * 5.f;

		character.emplace(physSytem.GetPhysWorld(), characterSettings);

		app.AddUpdaterFunc([&]
		{
			auto [position, rotation] = character->GetPositionAndRotation();

			auto& playerNode = playerEntity.get<Nz::NodeComponent>();
			playerNode.SetTransform(position, rotation, Nz::Vector3f::Unit());
		});

		std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(playerCollider->GenerateDebugMesh());
		std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

		std::shared_ptr<Nz::MaterialInstance> colliderMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
		colliderMat->SetValueProperty("BaseColor", Nz::Color::Green());
		colliderMat->UpdatePassesStates([](Nz::RenderStates& states)
		{
			states.primitiveMode = Nz::PrimitiveMode::LineList;
			return true;
		});

		auto colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
		for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
			colliderModel->SetMaterial(i, colliderMat);

		auto& playerGfx = playerEntity.emplace<Nz::GraphicsComponent>();
		playerGfx.AttachRenderable(std::move(colliderModel));

		auto& playerRotNode = playerRotation.emplace<Nz::NodeComponent>();
		playerRotNode.SetParent(playerNode);

		auto& cameraNode = playerCamera.emplace<Nz::NodeComponent>();
		cameraNode.SetPosition(Nz::Vector3f::Up() * 2.f + Nz::Vector3f::Backward());
		//cameraNode.SetParent(playerRotNode);

		auto& cameraComponent = playerCamera.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain));
		cameraComponent.UpdateZNear(0.2f);
		cameraComponent.UpdateZFar(10000.f);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	Nz::MeshParams meshParams;
	meshParams.animated = true;
	meshParams.center = true;
	meshParams.vertexScale = Nz::Vector3f(0.1f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent_Skinning);

	std::shared_ptr<Nz::Mesh> bobMesh = fs.Load<Nz::Mesh>("assets/character/Gangnam Style.fbx", meshParams);
	if (!bobMesh)
	{
		NazaraError("failed to load bob mesh");
		return __LINE__;
	}

	Nz::AnimationParams animParam;
	animParam.skeleton = bobMesh->GetSkeleton();
	animParam.jointRotation = meshParams.vertexRotation;
	animParam.jointScale = meshParams.vertexScale;
	animParam.jointOffset = meshParams.vertexOffset;

	std::shared_ptr<Nz::Animation> bobAnim = fs.Load<Nz::Animation>("assets/character/Gangnam Style.fbx", animParam);
	if (!bobAnim)
	{
		NazaraError("failed to load bob anim");
		return __LINE__;
	}

	std::shared_ptr<Nz::Skeleton> skeleton = std::make_shared<Nz::Skeleton>(*bobMesh->GetSkeleton());

	std::cout << "joint count: " << skeleton->GetJointCount() << std::endl;

	std::shared_ptr<Nz::GraphicalMesh> bobGfxMesh = Nz::GraphicalMesh::BuildFromMesh(*bobMesh);
	std::shared_ptr<Nz::Model> bobModel = std::make_shared<Nz::Model>(std::move(bobGfxMesh));

	std::vector<std::shared_ptr<Nz::MaterialInstance>> materials(bobMesh->GetMaterialCount());

	std::bitset<5> alphaMaterials("01010");
	for (std::size_t i = 0; i < bobMesh->GetMaterialCount(); ++i)
	{
		const Nz::ParameterList& materialData = bobMesh->GetMaterialData(i);
		std::string matPath = materialData.GetStringParameter(Nz::MaterialData::BaseColorTexturePath).GetValueOr("");
		if (!matPath.empty())
		{
			Nz::MaterialInstanceParams params;
			params.materialType = Nz::MaterialType::Phong;
			params.custom.SetParameter("sRGB", true);
			if (alphaMaterials.test(i))
				params.custom.SetParameter("EnableAlphaBlending", true);

			materials[i] = Nz::MaterialInstance::LoadFromFile(matPath, params);
		}
		else
			materials[i] = Nz::MaterialInstance::GetDefault(Nz::MaterialType::Basic);
	}

	for (std::size_t i = 0; i < bobMesh->GetSubMeshCount(); ++i)
	{
		std::size_t matIndex = bobMesh->GetSubMesh(i)->GetMaterialIndex();
		if (materials[matIndex])
			bobModel->SetMaterial(i, materials[matIndex]);
	}

	bool paused = false;

	/*for (std::size_t y = 0; y < 10; ++y)
	{
		for (std::size_t x = 0; x < 10; ++x)
		{
			entt::handle bobEntity = world.CreateEntity();

			auto& bobNode = bobEntity.emplace<Nz::NodeComponent>();
			bobNode.SetPosition(Nz::Vector3f(x - 5.f, 0.f, -float(y)));
			//bobNode.SetRotation(Nz::EulerAnglesf(-90.f, -90.f, 0.f));
			//bobNode.SetScale(1.f / 40.f * 0.5f);

			auto& bobGfx = bobEntity.emplace<Nz::GraphicsComponent>();
			bobGfx.AttachRenderable(bobModel);

			auto& sharedSkeleton = bobEntity.emplace<Nz::SharedSkeletonComponent>(skeleton);
		}
	}*/

	entt::handle bobEntity = world.CreateEntity();
	entt::handle lightEntity1 = world.CreateEntity();
	{
		auto& lightNode = lightEntity1.emplace<Nz::NodeComponent>();
		lightNode.SetPosition(Nz::Vector3f::Up() * 3.5f + Nz::Vector3f::Right() * 1.f);
		lightNode.SetRotation(Nz::EulerAnglesf(-70.f, 90.f, 0.f));

		auto& cameraLight = lightEntity1.emplace<Nz::LightComponent>();

		auto& spotLight = cameraLight.AddLight<Nz::SpotLight>();
		spotLight.UpdateColor(Nz::Color::Red());
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateShadowMapSize(1024);
	}

	entt::handle lightEntity2 = world.CreateEntity();
	{
		auto& lightNode = lightEntity2.emplace<Nz::NodeComponent>();
		lightNode.SetPosition(Nz::Vector3f::Up() * 3.5f + Nz::Vector3f::Right() * 1.5f);
		lightNode.SetRotation(Nz::EulerAnglesf(-70.f, 90.f, 0.f));

		auto& cameraLight = lightEntity2.emplace<Nz::LightComponent>();

		auto& spotLight = cameraLight.AddLight<Nz::SpotLight>();
		spotLight.UpdateColor(Nz::Color::Green());
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateShadowMapSize(1024);
	}

	entt::handle lightEntity3 = world.CreateEntity();

	{
		[[maybe_unused]] auto& bobNode = bobEntity.emplace<Nz::NodeComponent>();
		//bobNode.SetRotation(Nz::EulerAnglesf(-90.f, -90.f, 0.f));
		//bobNode.SetScale(1.f / 40.f * 0.5f);
		//bobNode.SetPosition(Nz::Vector3f(0.f, -1.f, 0.f));

		auto& bobGfx = bobEntity.emplace<Nz::GraphicsComponent>();
		bobGfx.AttachRenderable(bobModel);

		bobEntity.emplace<Nz::SharedSkeletonComponent>(skeleton);

		entt::handle sphereEntity = world.CreateEntity();
		{
			std::shared_ptr<Nz::Mesh> sphereMesh = std::make_shared<Nz::Mesh>();
			sphereMesh->CreateStatic();
			sphereMesh->BuildSubMesh(Nz::Primitive::UVSphere(1.f, 50, 50));
			sphereMesh->SetMaterialCount(1);

			std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*sphereMesh);

			// Textures
			Nz::TextureParams srgbTexParams;
			srgbTexParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

			std::shared_ptr<Nz::MaterialInstance> sphereMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
			sphereMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/Rusty/rustediron2_basecolor.png", srgbTexParams));

			std::shared_ptr<Nz::Model> sphereModel = std::make_shared<Nz::Model>(std::move(gfxMesh));
			for (std::size_t i = 0; i < sphereModel->GetSubMeshCount(); ++i)
				sphereModel->SetMaterial(i, sphereMat);

			auto& sphereNode = sphereEntity.emplace<Nz::NodeComponent>();
			sphereNode.SetScale(0.1f);
			sphereNode.SetInheritScale(false);
			sphereNode.SetParentJoint(bobEntity, "RightHand");

			auto& sphereGfx = sphereEntity.emplace<Nz::GraphicsComponent>();
			sphereGfx.AttachRenderable(sphereModel);
		}

		entt::handle smallBobEntity = world.CreateEntity();
		auto& smallBobNode = smallBobEntity.emplace<Nz::NodeComponent>();
		smallBobNode.SetParentJoint(bobEntity, "LeftHand");

		auto& smallBobGfx = smallBobEntity.emplace<Nz::GraphicsComponent>();
		smallBobGfx.AttachRenderable(bobModel);

		smallBobEntity.emplace<Nz::SharedSkeletonComponent>(skeleton);

		{
			lightEntity3.emplace<Nz::NodeComponent>();
			app.AddUpdaterFunc([&, lightEntity3, rotation = Nz::TurnAnglef::Zero()](Nz::Time deltaTime) mutable
			{
				if (paused)
					return;

				constexpr float radius = 3.5f;

				rotation += deltaTime.AsSeconds() * 0.5f;

				auto [sin, cos] = rotation.GetSinCos();

				auto& lightNode = lightEntity3.get<Nz::NodeComponent>();
				lightNode.SetPosition(sin * radius, 1.5f, cos * radius);
			});

			auto& cameraLight = lightEntity3.emplace<Nz::LightComponent>();

			auto& pointLight = cameraLight.AddLight<Nz::PointLight>();
			pointLight.UpdateColor(Nz::Color::White());
			pointLight.UpdateRadius(15.f);
			pointLight.EnableShadowCasting(true);
			pointLight.UpdateShadowMapSize(2048);
		}
	}

	std::shared_ptr<Nz::MaterialInstance> textMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
	textMat->UpdatePassesStates([](Nz::RenderStates& renderStates)
	{
		renderStates.faceCulling = Nz::FaceCulling::None;
		return true;
	});

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

	entt::handle textEntity = world.CreateEntity();
	{
		auto& entityGfx = textEntity.emplace<Nz::GraphicsComponent>();
		entityGfx.AttachRenderable(sprite, 1);

		auto& entityNode = textEntity.emplace<Nz::NodeComponent>();
		entityNode.SetPosition(Nz::Vector3f::Up() * 0.5f + Nz::Vector3f::Backward() * 0.66f + Nz::Vector3f::Left() * 0.5f);
		entityNode.SetRotation(Nz::EulerAnglesf(-45.f, 0.f, 0.f));
	}

	entt::handle floorEntity = world.CreateEntity();
	{
		Nz::MeshParams meshPrimitiveParams;
		meshPrimitiveParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

		Nz::Vector2f planeSize(25.f, 25.f);

		std::shared_ptr<Nz::GraphicalMesh> planeMeshGfx = Nz::GraphicalMesh::Build(Nz::Primitive::Plane(planeSize, Nz::Vector2ui(0u), Nz::Matrix4f::Identity(), Nz::Rectf(0.f, 0.f, 10.f, 10.f)), meshPrimitiveParams);

		Nz::TextureSamplerInfo planeSampler;
		planeSampler.anisotropyLevel = 16;
		planeSampler.wrapModeU = Nz::SamplerWrap::Repeat;
		planeSampler.wrapModeV = Nz::SamplerWrap::Repeat;

		Nz::TextureParams srgbTexParams;
		srgbTexParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

		std::shared_ptr<Nz::MaterialInstance> planeMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		planeMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/dev_grey.png", srgbTexParams), planeSampler);

		std::shared_ptr<Nz::Model> planeModel = std::make_shared<Nz::Model>(std::move(planeMeshGfx));
		planeModel->SetMaterial(0, planeMat);

		auto& planeGfx = floorEntity.emplace<Nz::GraphicsComponent>();
		planeGfx.AttachRenderable(planeModel);

		floorEntity.emplace<Nz::NodeComponent>();

		auto floorCollider = std::make_shared<Nz::BoxCollider3D>(Nz::Vector3f(planeSize.x, 1.f, planeSize.y));
		auto translatedFloorCollider = std::make_shared<Nz::TranslatedRotatedCollider3D>(std::move(floorCollider), Nz::Vector3f::Down() * 0.5f);

		Nz::RigidBody3D::StaticSettings floorSettings;
		floorSettings.geom = translatedFloorCollider;

		floorEntity.emplace<Nz::RigidBody3DComponent>(floorSettings);

		std::shared_ptr<Nz::GraphicalMesh> boxMeshGfx = Nz::GraphicalMesh::Build(Nz::Primitive::Box(Nz::Vector3f(0.5f, 0.5f, 0.5f)), meshPrimitiveParams);

		std::shared_ptr<Nz::Model> boxModel = std::make_shared<Nz::Model>(std::move(boxMeshGfx));
		boxModel->SetMaterial(0, planeMat);

		std::array cubePos = {
			Nz::Vector3f(0.904f, 0.25f, -0.4f),
			Nz::Vector3f(-2.04f, 0.25f, -1.149f),
			Nz::Vector3f(-0.05f, 0.25f, -0.922f),
			Nz::Vector3f(-2.586f, 0.25f, 0.892f),
		};

		for (const Nz::Vector3f& position : cubePos)
		{
			entt::handle boxEntity = world.CreateEntity();
			boxEntity.emplace<Nz::NodeComponent>(position);
			boxEntity.emplace<Nz::GraphicsComponent>(boxModel);
		}

		std::shared_ptr<Nz::Model> colliderModel;
		{
			std::shared_ptr<Nz::MaterialInstance> colliderMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
			colliderMat->SetValueProperty("BaseColor", Nz::Color::Green());
			colliderMat->UpdatePassesStates([](Nz::RenderStates& states)
			{
				states.primitiveMode = Nz::PrimitiveMode::LineList;
				return true;
			});

			std::shared_ptr<Nz::Mesh> colliderMesh = Nz::Mesh::Build(translatedFloorCollider->GenerateDebugMesh());
			std::shared_ptr<Nz::GraphicalMesh> colliderGraphicalMesh = Nz::GraphicalMesh::BuildFromMesh(*colliderMesh);

			colliderModel = std::make_shared<Nz::Model>(colliderGraphicalMesh);
			for (std::size_t i = 0; i < colliderModel->GetSubMeshCount(); ++i)
				colliderModel->SetMaterial(i, colliderMat);

			planeGfx.AttachRenderable(std::move(colliderModel));
		}
	}

	std::shared_ptr<Nz::Texture> screenTexture;
	{
		Nz::TextureInfo screenTextureInfo = {
			.pixelFormat = Nz::PixelFormat::RGBA8,
			.type = Nz::ImageType::E2D,
			.usageFlags = Nz::TextureUsage::ColorAttachment | Nz::TextureUsage::ShaderSampling | Nz::TextureUsage::TransferDestination,
			.levelCount = 1,
			.height = 360,
			.width = 480
		};

		std::size_t size = Nz::PixelFormatInfo::ComputeSize(screenTextureInfo.pixelFormat, screenTextureInfo.width, screenTextureInfo.height, screenTextureInfo.depth);

		std::vector<std::uint8_t> defaultScreen(size, 0xFF);
		screenTexture = device->InstantiateTexture(screenTextureInfo, defaultScreen.data(), false);
	}

	entt::handle tvCameraEntity = world.CreateEntity();
	{
		auto& cameraNode = tvCameraEntity.emplace<Nz::NodeComponent>();
		//cameraNode.SetParentJoint(bobEntity, "Head");
		//cameraNode.SetRotation(Nz::EulerAnglesf(-30.f, 180.f, 0.f));
		cameraNode.SetParent(playerCamera);

		auto& cameraComponent = tvCameraEntity.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderTexture>(screenTexture));
		cameraComponent.UpdateZNear(0.2f);
		cameraComponent.UpdateZFar(1000.f);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.f, 0.f, 0.f));
		cameraComponent.UpdateRenderOrder(-1);
	}


	entt::handle tvEntity = world.CreateEntity();
	{
		Nz::MeshParams tvMeshParams;
		tvMeshParams.vertexScale = Nz::Vector3f(0.01f);

		std::shared_ptr<Nz::Mesh> tvMesh = fs.Load<Nz::Mesh>("assets/retro_tv_lowpoly_4k_textures/scene.gltf", tvMeshParams);
		if (!tvMesh)
		{
			NazaraError("failed to load tv mesh");
			return __LINE__;
		}

		tvMesh->RemoveSubMesh(1); // looks like this submesh has issues loading

		// Screen UVs are upside down (model made for OpenGL?)
		{
			// Reverse them
			Nz::VertexMapper vertexMapper(*tvMesh->GetSubMesh(1));
			Nz::SparsePtr<Nz::Vector2f> uv = vertexMapper.GetComponentPtr<Nz::Vector2f>(Nz::VertexComponent::TexCoord);
			for (Nz::UInt32 i = 0; i < vertexMapper.GetVertexCount(); ++i)
				uv[i].y = 1.f - uv[i].y;
		}

		std::shared_ptr<Nz::GraphicalMesh> tvGfxMesh = Nz::GraphicalMesh::BuildFromMesh(*tvMesh);
		std::shared_ptr<Nz::Model> tvModel = std::make_shared<Nz::Model>(std::move(tvGfxMesh));

		Nz::TextureParams srgbTexParams;
		srgbTexParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

		std::shared_ptr<Nz::MaterialInstance> tvMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::PhysicallyBased);
		tvMat->SetTextureProperty("BaseColorMap", fs.Load<Nz::Texture>("assets/retro_tv_lowpoly_4k_textures/textures/old_tv_baseColor.jpeg", srgbTexParams));
		tvMat->SetTextureProperty("NormalMap", fs.Load<Nz::Texture>("assets/retro_tv_lowpoly_4k_textures/textures/old_tv_normal.png"));

		tvModel->SetMaterial(0, tvMat);

		std::shared_ptr<Nz::MaterialInstance> screenMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::PhysicallyBased);
		screenMat->SetTextureProperty("BaseColorMap", screenTexture);

		tvModel->SetMaterial(1, screenMat);

		auto& tvNode = tvEntity.emplace<Nz::NodeComponent>();
		tvNode.SetPosition(Nz::Vector3f(-2.586f, 0.5f, 0.892f));

		auto& tvGfx = tvEntity.emplace<Nz::GraphicsComponent>();
		tvGfx.AttachRenderable(tvModel);
	}

	Nz::MillisecondClock fpsClock, updateClock;
	float incr = 0.f;
	unsigned int currentFrame = 0;
	unsigned int nextFrame = 1;
	Nz::EulerAnglesf camAngles = Nz::EulerAnglesf(-30.f, 0.f, 0.f);
	Nz::UInt64 fps = 0;

	Nz::WindowEventHandler& eventHandler = mainWindow.GetEventHandler();
	eventHandler.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& event)
	{
		if (event.virtualKey == Nz::Keyboard::VKey::P)
			paused = !paused;
	});

	eventHandler.OnMouseMoved.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		// Gestion de la caméra free-fly (Rotation)
		float sensitivity = 0.3f; // Sensibilité de la souris

		// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
		camAngles.yaw = camAngles.yaw - event.deltaX * sensitivity;
		camAngles.yaw.Normalize();

		// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
		camAngles.pitch = Nz::Clamp(camAngles.pitch - event.deltaY * sensitivity, -89.f, 89.f);

		/*auto& playerRotNode = registry.get<Nz::NodeComponent>(playerRotation);
		playerRotNode.SetRotation(camAngles);*/
		auto& playerRotNode = playerCamera.get<Nz::NodeComponent>();
		playerRotNode.SetRotation(camAngles);
	});

	app.AddUpdaterFunc([&]
	{
		if (std::optional<Nz::Time> deltaTime = updateClock.RestartIfOver(Nz::Time::TickDuration(60)))
		{
			float updateTime = deltaTime->AsSeconds();

			//auto& playerBody = playerEntity.get<Nz::RigidBody3DComponent>();
			//playerBody.SetAngularDamping(std::numeric_limits<float>::max());

			Nz::Vector3f velocity = character->GetLinearVelocity();
			velocity.x = 0.f;
			velocity.z = 0.f;

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
			{
				if (character->IsOnGround())
					velocity += Nz::Vector3f::Up() * 2.f;
			}

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up))
				velocity += Nz::Vector3f::Forward();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down))
				velocity += Nz::Vector3f::Backward();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left))
				velocity += Nz::Vector3f::Left();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right))
				velocity += Nz::Vector3f::Right();

			character->SetLinearVelocity(velocity);

			float cameraSpeed = 2.f;

			auto& cameraNode = playerCamera.get<Nz::NodeComponent>();
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Space))
				cameraNode.Move(Nz::Vector3f::Up() * cameraSpeed * updateTime, Nz::CoordSys::Global);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				cameraNode.Move(Nz::Vector3f::Forward() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				cameraNode.Move(Nz::Vector3f::Backward() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				cameraNode.Move(Nz::Vector3f::Left() * cameraSpeed * updateTime, Nz::CoordSys::Local);

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
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

				//std::cout << currentFrame << std::endl;

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
		auto& lightNode = lightEntity3.get<Nz::NodeComponent>();
		//debugDrawer.DrawLine(lightNode.GetPosition(Nz::CoordSys::Global), lightNode.GetForward() * 10.f, Nz::Color::Blue());
		Nz::Vector3f pos = lightNode.GetPosition(Nz::CoordSys::Global);
		debugDrawer.DrawPoint(pos, Nz::Color::Blue());
		/*debugDrawer.DrawBox(floorBox, Nz::Color::Red);
		Nz::Boxf intersection;
		if (floorBox.Intersect(test, &intersection))
			debugDrawer.DrawBox(intersection, Nz::Color::Green());*/

		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			mainWindow.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetAliveEntityCount()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
}
