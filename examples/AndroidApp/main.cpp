#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/WindowingAppComponent.hpp>
#include <Nazara/Renderer.hpp>
#include <android/native_activity.h>
#include <android/log.h>
#include <iostream>
#include <random>

#if 0
int main()
{
	Nz::Renderer::Config rendererConfig;
	rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL_ES;

	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Application<Nz::Graphics> app(rendererConfig);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), "Hello Android");

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

	auto& renderSystem = world.AddSystem<Nz::RenderSystem>();
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	// Création de la caméra
	entt::handle cameraEntity = world.CreateEntity();
	{
		cameraEntity.emplace<Nz::NodeComponent>();

		auto& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(&windowSwapchain, Nz::ProjectionType::Perspective);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
		cameraComponent.UpdateFOV(Nz::DegreeAnglef(70.f));
	}

	// Création d'un texte
	Nz::SimpleTextDrawer textDrawer;
	textDrawer.SetText("Hello Android!");
	textDrawer.SetCharacterSize(72);
	textDrawer.SetOutlineThickness(4.f);

	std::shared_ptr<Nz::TextSprite> textSprite = std::make_shared<Nz::TextSprite>();
	textSprite->Update(textDrawer, 0.01f);

	entt::handle textEntity = world.CreateEntity();
	{
		auto& nodeComponent = textEntity.emplace<Nz::NodeComponent>();

		auto& gfxComponent = textEntity.emplace<Nz::GraphicsComponent>();
		gfxComponent.AttachRenderable(textSprite);

		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();
		nodeComponent.SetPosition(Nz::Vector3f::Forward() * 1.f + Nz::Vector3f::Left() * 1.f + Nz::Vector3f::Down() * 0.25f);
		nodeComponent.SetRotation(Nz::EulerAnglesf(0.f, 45.f, 0.f));
	}

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent);

	std::shared_ptr<Nz::Mesh> sphereMesh = std::make_shared<Nz::Mesh>();
	sphereMesh->CreateStatic();
	sphereMesh->BuildSubMesh(Nz::Primitive::UVSphere(0.1f, 50, 50));
	sphereMesh->SetMaterialCount(1);
	sphereMesh->GenerateNormalsAndTangents();

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*sphereMesh);

	// Textures
	std::shared_ptr<Nz::Material> material = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial;

	std::mt19937 rd;
	std::uniform_real_distribution<float> forwardDis(1.5f, 4.f);
	std::uniform_real_distribution<float> posDis(-2.f, 2.f);
	std::uniform_real_distribution<float> colorDis(0.f, 1.f);

	for (std::size_t i = 0; i < 500; ++i)
	{
		entt::handle sphereEntity = world.CreateEntity();
		{
			std::shared_ptr<Nz::MaterialInstance> materialInstance = std::make_shared<Nz::MaterialInstance>(material);
			materialInstance->SetValueProperty("BaseColor", Nz::Color(colorDis(rd), colorDis(rd), colorDis(rd)));

			std::shared_ptr<Nz::Model> model = std::make_shared<Nz::Model>(gfxMesh, sphereMesh->GetAABB());
			for (std::size_t i = 0; i < model->GetSubMeshCount(); ++i)
				model->SetMaterial(i, materialInstance);

			auto& nodeComponent = sphereEntity.emplace<Nz::NodeComponent>();

			auto& gfxComponent = sphereEntity.emplace<Nz::GraphicsComponent>();
			gfxComponent.AttachRenderable(model);

			nodeComponent.SetPosition(Nz::Vector3f::Forward() * forwardDis(rd) + Nz::Vector3f::Left() * posDis(rd) + Nz::Vector3f::Up() * posDis(rd));
		}
	}

	entt::handle lightEntity = world.CreateEntity();
	{
		auto& nodeComponent = lightEntity.emplace<Nz::NodeComponent>();
		nodeComponent.SetPosition(Nz::Vector3f::Forward() * 2.f);

		auto& lightComponent = lightEntity.emplace<Nz::LightComponent>();
		lightComponent.AddLight<Nz::PointLight>();
	}

	/*mainWindow.GetEventHandler().OnMouseButtonPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		auto& nodeComponent = textEntity.get<Nz::NodeComponent>();
		auto& gfxComponent = textEntity.get<Nz::GraphicsComponent>();
		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();
		nodeComponent.SetPosition(event.x - textBox.width / 2, windowSize.y - event.y - textBox.height / 2);
	});*/

	app.AddUpdater([&, counter = Nz::Time::Zero(), fps = 0](Nz::Time elapsedTime) mutable
	{
		fps++;
		counter += elapsedTime;
		if (counter >= Nz::Time::Second())
		{
			textDrawer.SetText("Hello Android!\n(" + std::to_string(fps) + " FPS)");
			textSprite->Update(textDrawer, 0.01f);

			counter -= Nz::Time::Second();
			fps = 0;
		}
	});

	return app.Run();
}
#endif

int main()
{
	Nz::Renderer::Config rendererConfig;
	rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL_ES;

	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Application<Nz::Graphics> app(rendererConfig);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	std::string windowTitle = "Graphics Test";
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), windowTitle);
	Nz::WindowSwapchain windowSwapchain(device, mainWindow);

	auto& fs = app.AddComponent<Nz::FilesystemAppComponent>();
	{
#ifdef NAZARA_PLATFORM_ANDROID
		fs.Mount("assets", std::make_shared<Nz::VirtualDirectory>(std::make_shared<Nz::AndroidAssetDirResolver>("examples")));
#else
		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		fs.Mount("assets", resourceDir);
#endif
	}

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::Mesh> spaceshipMesh = fs.Load<Nz::Mesh>("assets/Spaceship/spaceship.obj", meshParams);
	if (!spaceshipMesh)
	{
		NazaraError("Failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*spaceshipMesh);

	// Texture
	Nz::TextureParams texParams;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	std::shared_ptr<Nz::Texture> diffuseTexture = fs.Load<Nz::Texture>("assets/Spaceship/Texture/diffuse.png", texParams);

	std::shared_ptr<Nz::MaterialInstance> materialInstance = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
	materialInstance->SetTextureProperty(0, diffuseTexture);
	materialInstance->SetValueProperty(0, Nz::Color::White());

	std::shared_ptr<Nz::MaterialInstance> materialInstance2 = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic);
	materialInstance2->SetValueProperty(0, Nz::Color::Green());

	Nz::Model model(std::move(gfxMesh));
	for (std::size_t i = 0; i < model.GetSubMeshCount(); ++i)
		model.SetMaterial(i, materialInstance);

	Nz::Vector2ui windowSize = mainWindow.GetSize();

	Nz::Camera camera(std::make_shared<Nz::RenderWindow>(windowSwapchain));
	camera.UpdateClearColor(Nz::Color::Gray());

	Nz::ViewerInstance& viewerInstance = camera.GetViewerInstance();
	viewerInstance.UpdateTargetSize(Nz::Vector2f(mainWindow.GetSize()));
	viewerInstance.UpdateProjViewMatrices(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f), Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1));
	viewerInstance.UpdateNearFarPlanes(0.1f, 1000.f);

	Nz::WorldInstancePtr modelInstance = std::make_shared<Nz::WorldInstance>();
	modelInstance->UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Left()));

	Nz::WorldInstancePtr modelInstance2 = std::make_shared<Nz::WorldInstance>();
	modelInstance2->UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Right()));

	Nz::Recti scissorBox(Nz::Vector2i::Zero(), Nz::Vector2i(mainWindow.GetSize()));

	Nz::ElementRendererRegistry elementRegistry;
	Nz::ForwardFramePipeline framePipeline(elementRegistry);
	std::size_t cameraIndex = framePipeline.RegisterViewer(&camera, 0);
	std::size_t worldInstanceIndex1 = framePipeline.RegisterWorldInstance(modelInstance);
	std::size_t worldInstanceIndex2 = framePipeline.RegisterWorldInstance(modelInstance2);
	framePipeline.RegisterRenderable(worldInstanceIndex1, Nz::FramePipeline::NoSkeletonInstance, &model, 0xFFFFFFFF, scissorBox);
	framePipeline.RegisterRenderable(worldInstanceIndex2, Nz::FramePipeline::NoSkeletonInstance, &model, 0xFFFFFFFF, scissorBox);

	std::unique_ptr<Nz::SpotLight> light = std::make_unique<Nz::SpotLight>();
	light->UpdateInnerAngle(Nz::DegreeAnglef(15.f));
	light->UpdateOuterAngle(Nz::DegreeAnglef(20.f));

	framePipeline.RegisterLight(light.get(), 0xFFFFFFFF);

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock updateClock;
	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

	mainWindow.GetEventHandler().OnEvent.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent& event)
	{
		switch (event.type)
		{
			case Nz::WindowEventType::Quit:
				mainWindow.Close();
				break;

			case Nz::WindowEventType::Resized:
			{
				Nz::Vector2ui newWindowSize = mainWindow.GetSize();
				viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(newWindowSize.x) / newWindowSize.y, 0.1f, 1000.f));
				viewerInstance.UpdateTargetSize(Nz::Vector2f(newWindowSize));
				break;
			}

			default:
				break;
		}
	});

	app.AddUpdaterFunc([&]
	{
		Nz::RenderFrame frame = windowSwapchain.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			return;
		}

		for (const Nz::WorldInstancePtr& worldInstance : { modelInstance, modelInstance2 })
		{
			Nz::Boxf aabb = model.GetAABB();
			aabb.Transform(worldInstance->GetWorldMatrix());

			framePipeline.GetDebugDrawer().DrawBox(aabb, Nz::Color::Green());
		}

		viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(viewerPos, camAngles));
		viewerInstance.UpdateEyePosition(viewerPos);

		framePipeline.Render(frame);

		frame.Present();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			mainWindow.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");
			fps = 0;
		}
	});

	return app.Run();
}

JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, std::size_t savedStateSize)
{
	__android_log_print(ANDROID_LOG_WARN, "NazaraApp", "Hello Nazara!");

	static Nz::AndroidActivity nazaraAndroid(activity, savedState, savedStateSize);
	nazaraAndroid.Start(main);
}
