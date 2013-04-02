// Cette démo suppose que vous avez déjà lu les commentaires de "FirstScene"

#include <Nazara/Core/Clock.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <iostream>

int main()
{
	NzInitializer<NzGraphics> nazara;
	if (!nazara)
	{
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// Cette démo s'appuie sur l'utilisation des RenderTexture pour simuler le 16bits
	if (!NzRenderTexture::IsSupported())
	{
		std::cout << "Render textures are not supported by your system" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	NzScene scene;

	NzModel drfreak;
	if (!drfreak.LoadFromFile("resources/drfreak.md2"))
	{
		std::cout << "Failed to load Dr. Freak" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	std::vector<NzModel> models(100, drfreak);
	for (unsigned int i = 0; i < models.size(); ++i)
	{
		models[i].SetPosition(i/10 * 40, 0.f, i%10 * 40);
		models[i].SetParent(scene);
	}

	NzEulerAnglesf camAngles(-45.f, 180.f, 0.f);

	NzCamera camera;
	camera.SetPosition(200.f, 50.f, 200);
	camera.SetRotation(camAngles);
	camera.SetParent(scene);
	camera.SetZFar(500.f);
	camera.SetZNear(1.f);

	NzLight spotLight(nzLightType_Spot);

	spotLight.SetParent(camera);

	// Comme dans "FirstScene", notre fenêtre aura une résolution de 75% de celle du bureau
	NzVideoMode mode = NzVideoMode::GetDesktopMode();
	mode.width *= 3.f/4.f;
	mode.height *= 3.f/4.f;
	mode.width = 1280;
	mode.height = 720;

	NzString windowTitle = "Nazara Demo - 16bits rendering";

	NzRenderWindow window(mode, windowTitle);
	if (!window.IsValid())
	{
		std::cout << "Failed to create render window" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	window.SetCursor(nzWindowCursor_None);

	// Création d'une RenderTexture (RTT)
	NzRenderTexture renderTexture;

	// On choisit une dimension cinq fois plus petite que celle de la fenêtre (pour un effet pixelisé)
	unsigned int width = window.GetWidth()/5;
	unsigned int height = window.GetHeight()/5;

	// Le dernier paramètre signifie qu'on verrouille la RenderTexture pour lui appliquer d'autres opérations immédiatement
	if (!renderTexture.Create(width, height, true))
	{
		std::cout << "Failed to create render texture" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// Ensuite on attache un depth buffer (nécessaire pour le rendu)
	if (!renderTexture.AttachBuffer(nzAttachmentPoint_Depth, 0, nzPixelFormat_Depth24))
	{
		std::cout << "Failed to attach depth buffer to render texture" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// La texture qui va recevoir le rendu
	NzTexture targetTexture;

	// On va créer une texture 16bits pour limiter le nombre de couleurs
	if (!targetTexture.Create(nzImageType_2D, nzPixelFormat_RGBA4, width, height))
	{
		std::cout << "Failed to create render texture target" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// On attache ensuite la texture au RTT
	if (!renderTexture.AttachTexture(nzAttachmentPoint_Color, 0, &targetTexture))
	{
		std::cout << "Failed to attach texture target to render texture" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// Ensuite on vérifie que le RTT est complet (Que la carte graphique l'accepte)
	// Cela peut arriver s'il n'y a pas de point d'attache, ou que le format d'une texture n'est pas supporté pour le rendu
	if (!renderTexture.IsComplete())
	{
		std::cout << "Failed to build RTT" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// On a terminé avec la RenderTexture, on oublie surtout pas de la déverouiller
	// (J'ai passé une heure à essayer de comprendre pourquoi la démo ne fonctionnait pas à cause de ça)
	renderTexture.Unlock();

	// On attache la caméra au RTT (Le rendu se fera sur la texture)
	camera.SetTarget(renderTexture);

	// Pour avoir un effet de pixelisation au lieu d'un effet de flou, on désactive le filtrage
	NzTextureSampler::SetDefaultFilterMode(nzSamplerFilter_Nearest);

	NzClock secondClock, updateClock;

	unsigned int fps = 0;

	while (window.IsOpen())
	{
		NzEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case nzEventType_MouseMoved:
				{
					float sensitivity = 0.8f;

					camAngles.yaw = NzNormalizeAngle(camAngles.yaw - event.mouseMove.deltaX*sensitivity);
					camAngles.pitch = NzClamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					camera.SetRotation(camAngles);

					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_Quit:
					window.Close();
					break;

				case nzEventType_KeyPressed:
					if (event.key.code == NzKeyboard::Key::Escape)
						window.Close();
					break;

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() >= 1000/60)
		{
			float cameraSpeed = 300.f;
			float elapsedTime = updateClock.GetSeconds();

			if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
				cameraSpeed *= 2.f;

			if (NzKeyboard::IsKeyPressed(NzKeyboard::Up) || NzKeyboard::IsKeyPressed(NzKeyboard::Z))
				camera.Move(NzVector3f::Forward() * cameraSpeed * elapsedTime);

			if (NzKeyboard::IsKeyPressed(NzKeyboard::Down) || NzKeyboard::IsKeyPressed(NzKeyboard::S))
				camera.Move(NzVector3f::Backward() * cameraSpeed * elapsedTime);

			if (NzKeyboard::IsKeyPressed(NzKeyboard::Left) || NzKeyboard::IsKeyPressed(NzKeyboard::Q))
				camera.Move(NzVector3f::Left() * cameraSpeed * elapsedTime);

			if (NzKeyboard::IsKeyPressed(NzKeyboard::Right) || NzKeyboard::IsKeyPressed(NzKeyboard::D))
				camera.Move(NzVector3f::Right() * cameraSpeed * elapsedTime);

			if (NzKeyboard::IsKeyPressed(NzKeyboard::LShift) || NzKeyboard::IsKeyPressed(NzKeyboard::RShift))
				camera.Move(NzVector3f::Up() * cameraSpeed * elapsedTime, nzCoordSys_Global);

			if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl) || NzKeyboard::IsKeyPressed(NzKeyboard::RControl))
				camera.Move(NzVector3f::Down() * cameraSpeed * elapsedTime, nzCoordSys_Global);

			updateClock.Restart();
		}

		camera.Activate();

		scene.Update();
		scene.Cull();
		scene.UpdateVisible();
		scene.Draw();

		// Le rendu de la scène est fait, mais dans la texture
		// Il nous reste donc à afficher (manuellement) la texture sur l'écran à l'aide du Renderer
		// Ceci sera fait par le module 2D plus tard
		// Alerte rouge: Ce code utilise "DrawTexture" qui est une fonction utilitaire non-terminée et aux performances médiocres
		// Elle n'existe et n'est utilisée que pour ne pas complexifier la démo
		// Elle sera grandement remaniée prochainement

		// On active la fenêtre pour le rendu
		NzRenderer::SetTarget(&window);

		// On précise que la zone de rendu occupe tout l'écran, important car on mêle du rendu manuel et automatique (module 3D)
		NzRenderer::SetViewport(NzRectui(0,0,window.GetWidth(), window.GetHeight()));

		// On vide le depth-buffer (On pourrait tout aussi bien le désactiver)
		NzRenderer::Clear(nzRendererClear_Depth);

		// Et on appelle notre fonction satanique qui affichera la texture sur toute la surface de la fenêtre
		NzRenderer::SetTexture(0, &targetTexture);
		NzRenderer::DrawTexture(0, NzRectf(0.f, 0.f, window.GetWidth(), window.GetHeight()), NzVector2f(0.f, 0.f), NzVector2f(1.f, 1.f));

		window.Display();

		fps++;

		if (secondClock.GetMilliseconds() >= 1000)
		{
			unsigned int visibleNode = 0;
			for (NzModel& model : models)
			{
				if (model.IsVisible())
					visibleNode++;
			}

			window.SetTitle(windowTitle + " - " + NzString::Number(fps) + " FPS - " + NzString::Number(visibleNode) + " modèles visibles");
			fps = 0;

			secondClock.Restart();
		}
	}

    return EXIT_SUCCESS;
}
