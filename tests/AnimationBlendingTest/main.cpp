#include <Nazara/Core.hpp>
#include <Nazara/Core/Plugins/AssimpPlugin.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Math/PidController.hpp>
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
	Nz::Application<Nz::Graphics> app(argc, argv);

	Nz::PluginLoader loader;
	Nz::Plugin<Nz::AssimpPlugin> assimp = loader.Load<Nz::AssimpPlugin>();

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	world.AddSystem<Nz::SkeletonSystem>();

	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	std::string windowTitle = "Skinning test";
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), windowTitle);
	auto& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	auto& fs = app.AddComponent<Nz::FilesystemAppComponent>();
	{
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
	}

	entt::handle playerCamera = world.CreateEntity();
	{
		auto& cameraNode = playerCamera.emplace<Nz::NodeComponent>();
		cameraNode.SetPosition(Nz::Vector3f::Up() * 2.f + Nz::Vector3f::Backward());

		auto& cameraComponent = playerCamera.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain));
		cameraComponent.UpdateZNear(0.2f);
		cameraComponent.UpdateZFar(1000.f);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	Nz::MeshParams meshParams;
	meshParams.animated = true;
	meshParams.center = true;
	meshParams.vertexScale = Nz::Vector3f(0.1f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent_Skinning);

	std::shared_ptr<Nz::Mesh> characterMesh = fs.Load<Nz::Mesh>("assets/character/Gangnam Style.fbx", meshParams);
	if (!characterMesh)
	{
		NazaraError("failed to load bob mesh");
		return __LINE__;
	}

	Nz::AnimationParams animParam;
	animParam.skeleton = characterMesh->GetSkeleton();
	animParam.jointRotation = meshParams.vertexRotation;
	animParam.jointScale = meshParams.vertexScale;
	animParam.jointOffset = meshParams.vertexOffset;

	std::shared_ptr<Nz::Animation> idleAnim = fs.Load<Nz::Animation>("assets/character/Neutral Idle.fbx", animParam);
	if (!idleAnim)
	{
		NazaraError("failed to load idle anim");
		return __LINE__;
	}

	std::shared_ptr<Nz::Animation> walkingAnim = fs.Load<Nz::Animation>("assets/character/Walking.fbx", animParam);
	if (!walkingAnim)
	{
		NazaraError("failed to load walking anim");
		return __LINE__;
	}

	std::shared_ptr<Nz::Animation> runningAnim = fs.Load<Nz::Animation>("assets/character/Running.fbx", animParam);
	if (!runningAnim)
	{
		NazaraError("failed to load running anim");
		return __LINE__;
	}

	std::shared_ptr<Nz::Animation> sprintAnim = fs.Load<Nz::Animation>("assets/character/Sprint.fbx", animParam);
	if (!runningAnim)
	{
		NazaraError("failed to load running anim");
		return __LINE__;
	}

	std::shared_ptr<Nz::Skeleton> skeleton = std::make_shared<Nz::Skeleton>(*characterMesh->GetSkeleton());

	Nz::AnimationBlender animBlender(*characterMesh->GetSkeleton());
	animBlender.AddPoint(0.f, idleAnim);
	animBlender.AddPoint(5.f, walkingAnim);
	animBlender.AddPoint(10.f, runningAnim);
	animBlender.AddPoint(20.f, sprintAnim, 0, 1.25f);

	std::shared_ptr<Nz::GraphicalMesh> bobGfxMesh = Nz::GraphicalMesh::BuildFromMesh(*characterMesh);
	std::shared_ptr<Nz::Model> bobModel = std::make_shared<Nz::Model>(std::move(bobGfxMesh));

	std::vector<std::shared_ptr<Nz::MaterialInstance>> materials(characterMesh->GetMaterialCount());

	std::bitset<4> alphaMaterials("0100");
	for (std::size_t i = 0; i < characterMesh->GetMaterialCount(); ++i)
	{
		const Nz::ParameterList& materialData = characterMesh->GetMaterialData(i);
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

	for (std::size_t i = 0; i < characterMesh->GetSubMeshCount(); ++i)
	{
		std::size_t matIndex = characterMesh->GetSubMesh(i)->GetMaterialIndex();
		if (materials[matIndex])
			bobModel->SetMaterial(i, materials[matIndex]);
	}

	bool paused = false;

	entt::handle lightEntity = world.CreateEntity();
	{
		lightEntity.emplace<Nz::NodeComponent>(Nz::Vector3f::Up() * 2.5f + Nz::Vector3f::Right() * 2.f + Nz::Vector3f::Backward() * 2.f, Nz::EulerAnglesf(-30.f, 45.f, 0.f));

		auto& lightData = lightEntity.emplace<Nz::LightComponent>();

		auto& spotLight = lightData.AddLight<Nz::SpotLight>();
		spotLight.EnableShadowCasting(true);
		spotLight.UpdateShadowMapSize(1024);
		spotLight.UpdateInnerAngle(Nz::DegreeAnglef(10.f));
		spotLight.UpdateOuterAngle(Nz::DegreeAnglef(15.f));
		spotLight.UpdateRadius(10.f);
	}

	entt::handle characterEntity = world.CreateEntity();
	{
		characterEntity.emplace<Nz::NodeComponent>(Nz::Vector3f::Zero(), Nz::EulerAnglesf(0.f, 90.f, 0.f));
		characterEntity.emplace<Nz::GraphicsComponent>(bobModel);
		characterEntity.emplace<Nz::SharedSkeletonComponent>(skeleton);
	}

	std::shared_ptr<Nz::TextSprite> sprite = std::make_shared<Nz::TextSprite>();
	sprite->Update(Nz::SimpleTextDrawer::Draw("Speed: 0km/h", 72), 0.002f);

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

		std::shared_ptr<Nz::MaterialInstance> planeMat = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
		planeMat->SetTextureProperty("BaseColorMap", fs.Open<Nz::TextureAsset>("assets/dev_grey.png", { .sRGB = true }), planeSampler);

		std::shared_ptr<Nz::Model> planeModel = std::make_shared<Nz::Model>(std::move(planeMeshGfx));
		planeModel->SetMaterial(0, planeMat);

		auto& planeGfx = floorEntity.emplace<Nz::GraphicsComponent>();
		planeGfx.AttachRenderable(planeModel);

		floorEntity.emplace<Nz::NodeComponent>();
	}


	Nz::WindowEventHandler& eventHandler = mainWindow.GetEventHandler();
	eventHandler.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& event)
	{
		if (event.virtualKey == Nz::Keyboard::VKey::P)
			paused = !paused;
	});

	Nz::EulerAnglesf camAngles = Nz::EulerAnglesf(-30.f, 0.f, 0.f);
	eventHandler.OnMouseMoved.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		// Gestion de la caméra free-fly (Rotation)
		float sensitivity = 0.3f; // Sensibilité de la souris

		camAngles.yaw = camAngles.yaw - event.deltaX * sensitivity;
		camAngles.yaw.Normalize();
		camAngles.pitch = Nz::Clamp(camAngles.pitch - event.deltaY * sensitivity, -89.f, 89.f);

		auto& playerRotNode = playerCamera.get<Nz::NodeComponent>();
		playerRotNode.SetRotation(camAngles);
	});

	animBlender.UpdateValue(1.f);

	unsigned int fps = 0;
	float speed = -5.f;
	app.AddUpdaterFunc([&](Nz::Time elapsedTime)
	{
		float updateTime = elapsedTime.AsSeconds();

		constexpr float cameraSpeed = 2.f;

		auto& cameraNode = playerCamera.get<Nz::NodeComponent>();
		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Space))
			cameraNode.MoveGlobal(Nz::Vector3f::Up() * cameraSpeed * updateTime);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
			cameraNode.Move(Nz::Vector3f::Forward() * cameraSpeed * updateTime);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
			cameraNode.Move(Nz::Vector3f::Backward() * cameraSpeed * updateTime);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
			cameraNode.Move(Nz::Vector3f::Left() * cameraSpeed * updateTime);

		if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
			cameraNode.Move(Nz::Vector3f::Right() * cameraSpeed * updateTime);

		if (!paused)
		{
			animBlender.UpdateAnimation(elapsedTime);
			animBlender.AnimateSkeleton(skeleton.get());
		}

		fps++;

		speed += updateTime;
		if (speed > 25.f)
			speed = -5.f;

		sprite->Update(Nz::SimpleTextDrawer::Draw(Nz::Format("Speed: {0:.2f}", std::max(speed, 0.f)), 72), 0.002f);
		animBlender.UpdateValue(speed);
	});

	app.AddUpdaterFunc(Nz::ApplicationBase::Interval{ Nz::Time::Second() }, [&]
	{
		mainWindow.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetAliveEntityCount()) + " entities");
		fps = 0;
	});

	return app.Run();
}
