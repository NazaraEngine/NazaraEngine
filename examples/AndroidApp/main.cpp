#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/AppWindowingComponent.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <android/native_activity.h>
#include <android/log.h>
#include <iostream>
#include <random>

int main()
{
	Nz::Renderer::Config rendererConfig;
	rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL_ES;

	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Application<Nz::Graphics> app(rendererConfig);

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1920, 1080), "Android Test");

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
		nodeComponent.SetPosition(Nz::Vector3f::Forward() * 1.f + Nz::Vector3f::Left() * 2.f + Nz::Vector3f::Down() * 0.25f);
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
	std::shared_ptr<Nz::Material> material = Nz::Graphics::Instance()->GetDefaultMaterials().basicMaterial;

	std::mt19937 rd;
	std::uniform_real_distribution<float> forwardDis(1.5f, 4.f);
	std::uniform_real_distribution<float> posDis(-2.f, 2.f);
	std::uniform_real_distribution<float> colorDis(0.f, 1.f);

	for (std::size_t i = 0; i < 300; ++i)
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
			textSprite->Update(textDrawer);

			counter -= Nz::Time::Second();
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
