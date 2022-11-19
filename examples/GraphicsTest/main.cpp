#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/PropertyHandler/TexturePropertyHandler.hpp>
#include <Nazara/Graphics/PropertyHandler/UniformValuePropertyHandler.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <thread>

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

	Nz::RenderWindow window;

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent);

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

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*spaceshipMesh);

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

	std::shared_ptr<Nz::Texture> diffuseTexture = Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams);

	std::shared_ptr<Nz::Material> material = Nz::Graphics::Instance()->GetDefaultMaterials().basicMaterial;

	std::shared_ptr<Nz::MaterialInstance> materialInstance = std::make_shared<Nz::MaterialInstance>(material);
	materialInstance->SetTextureProperty(0, diffuseTexture);
	materialInstance->SetValueProperty(0, Nz::Color::White);

	std::shared_ptr<Nz::MaterialInstance> materialInstance2 = std::make_shared<Nz::MaterialInstance>(material);
	materialInstance2->SetValueProperty(0, Nz::Color::Green);

	Nz::Model model(std::move(gfxMesh), spaceshipMesh->GetAABB());
	for (std::size_t i = 0; i < model.GetSubMeshCount(); ++i)
		model.SetMaterial(i, materialInstance);

	Nz::Vector2ui windowSize = window.GetSize();

	Nz::Camera camera(window.GetRenderTarget());
	camera.UpdateClearColor(Nz::Color::Gray);

	Nz::ViewerInstance& viewerInstance = camera.GetViewerInstance();
	viewerInstance.UpdateTargetSize(Nz::Vector2f(window.GetSize()));
	viewerInstance.UpdateProjViewMatrices(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f), Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1));

	Nz::WorldInstancePtr modelInstance = std::make_shared<Nz::WorldInstance>();
	modelInstance->UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Left()));

	Nz::WorldInstancePtr modelInstance2 = std::make_shared<Nz::WorldInstance>();
	modelInstance2->UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Right()));

	Nz::Recti scissorBox(Nz::Vector2i(window.GetSize()));

	Nz::ElementRendererRegistry elementRegistry;
	Nz::ForwardFramePipeline framePipeline(elementRegistry);
	std::size_t cameraIndex = framePipeline.RegisterViewer(&camera, 0);
	std::size_t worldInstanceIndex1 = framePipeline.RegisterWorldInstance(modelInstance);
	std::size_t worldInstanceIndex2 = framePipeline.RegisterWorldInstance(modelInstance2);
	framePipeline.RegisterRenderable(worldInstanceIndex1, Nz::FramePipeline::NoSkeletonInstance, &model, 0xFFFFFFFF, scissorBox);
	framePipeline.RegisterRenderable(worldInstanceIndex2, Nz::FramePipeline::NoSkeletonInstance, &model, 0xFFFFFFFF, scissorBox);

	std::shared_ptr<Nz::SpotLight> light = std::make_shared<Nz::SpotLight>();
	light->UpdateInnerAngle(Nz::DegreeAnglef(15.f));
	light->UpdateOuterAngle(Nz::DegreeAnglef(20.f));

	framePipeline.RegisterLight(light, 0xFFFFFFFF);

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

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
					if (event.key.virtualKey == Nz::Keyboard::VKey::A)
					{
						for (std::size_t i = 0; i < model.GetSubMeshCount(); ++i)
							model.SetMaterial(i, materialInstance);
					}
					else if (event.key.virtualKey == Nz::Keyboard::VKey::B)
					{
						for (std::size_t i = 0; i < model.GetSubMeshCount(); ++i)
							model.SetMaterial(i, materialInstance2);
					}
					else if (event.key.virtualKey == Nz::Keyboard::VKey::Space)
					{
						modelInstance->UpdateWorldMatrix(Nz::Matrix4f::Translate(viewerPos));
					}

					break;

				case Nz::WindowEventType::MouseMoved: // La souris a bougé
				{
					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX * sensitivity;
					camAngles.yaw.Normalize();

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					camQuat = camAngles;
					light->UpdateRotation(camQuat);
					break;
				}

				case Nz::WindowEventType::Resized:
				{
					Nz::Vector2ui newWindowSize = window.GetSize();
					viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(newWindowSize.x) / newWindowSize.y, 0.1f, 1000.f));
					viewerInstance.UpdateTargetSize(Nz::Vector2f(newWindowSize));
					break;
				}

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float cameraSpeed = 2.f * updateClock.GetSeconds();
			updateClock.Restart();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				viewerPos += camQuat * Nz::Vector3f::Forward() * cameraSpeed;

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				viewerPos += camQuat * Nz::Vector3f::Backward() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				viewerPos += camQuat * Nz::Vector3f::Left() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				viewerPos += camQuat * Nz::Vector3f::Right() * cameraSpeed;

			// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				viewerPos += Nz::Vector3f::Up() * cameraSpeed;

			// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				viewerPos += Nz::Vector3f::Down() * cameraSpeed;

			light->UpdatePosition(viewerPos);
		}

		Nz::RenderFrame frame = window.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		framePipeline.GetDebugDrawer().DrawLine(Nz::Vector3f::Zero(), Nz::Vector3f::Forward(), Nz::Color::Blue);

		viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(viewerPos, camAngles));
		viewerInstance.UpdateEyePosition(viewerPos);

		framePipeline.Render(frame);

		frame.Present();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (secondClock.GetMilliseconds() >= 1000) // Toutes les secondes
		{
			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");

			/*
			Note: En C++11 il est possible d'insérer de l'Unicode de façon standard, quel que soit l'encodage du fichier,
			via quelque chose de similaire à u8"Cha\u00CEne de caract\u00E8res".
			Cependant, si le code source est encodé en UTF-8 (Comme c'est le cas dans ce fichier),
			cela fonctionnera aussi comme ceci : "Chaîne de caractères".
			*/

			// Et on réinitialise le compteur de FPS
			fps = 0;

			// Et on relance l'horloge pour refaire ça dans une seconde
			secondClock.Restart();
		}
	}

	return EXIT_SUCCESS;
}
