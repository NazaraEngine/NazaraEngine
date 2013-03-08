#include <Nazara/3D.hpp> // Module 3D
#include <Nazara/Core/Clock.hpp> // Horloges
#include <Nazara/Renderer.hpp> // Module de rendu
#include <Nazara/Utility.hpp> // Module utilitaire
#include <iostream>

int main()
{
	// Pour commencer, nous initialisons le module 3D, celui-ci va provoquer l'initialisation (dans l'ordre),
	// du noyau (Core), Utility, Renderer, 2D.
	// NzInitializer est une classe RAII appelant Initialize dans son constructeur et Uninitialize dans son destructeur.
	// Autrement dit, une fois ceci fait nous n'avons plus à nous soucier de la libération du moteur.
	NzInitializer<Nz3D> nazara;
	if (!nazara)
	{
		// Une erreur s'est produite dans l'initialisation d'un des modules
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur

		return EXIT_FAILURE;
	}

	// Nazara étant initialisé, nous pouvons créer la scène
	// Une scène représente tout ce qui est visible par une ou plusieurs caméras.
	// La plupart du temps vous n'aurez pas besoin de plus d'une scène, mais cela peut se révéler utile pour mieux
	// organiser et optimiser le rendu.
	// Par exemple, une pièce contenant une télévision, laquelle affichant des images provenant d'une NzCamera
	// Le rendu sera alors plus efficace en créant deux scènes, une pour la pièce et l'autre pour les images de la télé.
	// Cela diminuera le nombre de SceneNode à gérer pour chaque scène, et vous permettra même de ne pas afficher la scène
	// affichée dans la télé si cette dernière n'est pas visible dans la première scène.
	NzScene scene;

	// Nous allons commencer par rajouter des modèles à notre scène
	// Nous choisirons l'éternel Dr. Freak (Qui ne peut plus être animé car les vieilles animations image-clé
	// ne sont plus supportées par le moteur pour des raisons techniques)
	NzModel drfreak;

	// On charge ensuite le modèle depuis un fichier .md2, le moteur va se charger d'essayer de retrouver les matériaux associés
	if (!drfreak.LoadFromFile("resources/drfreak.md2"))
	{
		std::cout << "Failed to load Dr. Freak" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// On rajoute également une normal-map externe car elle n'est pas précisée dans le format MD2
	// On l'alloue dynamiquement pour ne pas avoir de problème avec les ressources, car en effet, si la texture était supprimée
	// avant que le modèle ne le soit, alors il y aurait un crash lorsque le modèle supprimerait sa référence vers la texture
	NzTexture* normalMap = new NzTexture;
	if (normalMap->LoadFromFile("resources/drfreak_bump.tga"))
	{
		// On associe ensuite la normal map au matériau du Dr. Freak
		NzMaterial* material = drfreak.GetMaterial(0);
		material->SetNormalMap(normalMap);

		// On va rendre notre texture non-persistante, cela signifie que lorsque son compteur de référence tombera à zéro,
		// elle sera automatiquement libérée. (Ce qui sera le cas lorsque tous les modèles auront étés libérés)
		normalMap->SetPersistent(false);
	}
	else
	{
		delete normalMap;

		std::cout << "Failed to load normal map" << std::endl;
	}

	// Nous allons faire une centaine de copie du modèle du Dr. Freak, chaque modèle sera indépendant dans ses propriétés
	// à l'exception de son mesh, de ses matériaux et de son animation s'il en avait eu une, car ceux-ci sont des ressources
	// lourdes en mémoire (Contrairement au modèle) qui ne seront pas dupliqués mais référencés
	// Autrement dit, chaque modèle possède une référence vers le mesh et les matériaux du Dr. Freak original, si nous venions
	// à supprimer le mesh original, il n'y aurait aucun problème (Car les ressources sont toujours utilisées par les autres)
	std::vector<NzModel> models(100, drfreak);
	for (unsigned int i = 0; i < models.size(); ++i)
	{
		models[i].SetPosition(i/10 * 40, 0.f, i%10 * 40); // On les espace
		models[i].SetParent(scene); // Et on les attache à la scène
	}

	// Nous avons besoin également d'une caméra, pour des raisons évidentes, celle-ci sera placée au dessus des modèles
	// Et dans leur direction (Nous nous arrangerons également pour en faire une caméra free-fly via les évènements)

	// On conserve la rotation à part via des angles d'eulers pour la caméra free-fly
	NzEulerAnglesf camAngles(-45.f, 180.f, 0.f);

	NzCamera camera;
	camera.SetPosition(200.f, 50.f, 200); // On place la caméra au milieu de tous les modèles
	camera.SetRotation(camAngles);
	camera.SetParent(scene); // On l'attache également à la scène

	// Et on n'oublie pas de définir les plans délimitant le champs de vision
	// (Seul ce qui se trouvera entre les deux plans sera rendu)
	camera.SetZFar(500.f); // La distance entre l'oeil et le plan éloigné
	camera.SetZNear(1.f); // La distance entre l'oeil et le plan rapproché

	// Il ne nous manque plus maintenant que de l'éclairage, sans quoi la scène sera complètement noire
	NzLight spotLight(nzLightType_Spot);

	// On attache la lumière à la caméra pour qu'elle suive sa position et son orientation, ce qui va aussi l'attacher à la scène
	// car la caméra y est attachée
	spotLight.SetParent(camera);

	// Nous allons maintenant créer la fenêtre, dans laquelle nous ferons nos rendus
	// Celle-ci demande des paramètres un peu plus complexes

	// Pour commencer le mode vidéo, celui-ci va définir la taille de la zone de rendu et le nombre de bits par pixels
	NzVideoMode mode = NzVideoMode::GetDesktopMode(); // Nous récupérons le mode vidéo du bureau

	// Nous allons prendre les trois quarts de la résolution du bureau pour notre fenêtre
	mode.width *= 3.f/4.f;
	mode.height *= 3.f/4.f;

	// Maintenant le titre, rien de plus simple...
	NzString windowTitle = "Nazara Demo - First scene";

	NzRenderWindow window(mode, windowTitle);
	if (!window.IsValid())
	{
		std::cout << "Failed to create render window" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// On fait disparaître le curseur de la souris
	window.SetCursor(nzWindowCursor_None);

	// On lie la caméra à la fenêtre
	camera.SetTarget(window);

	// Et on créé deux horloges pour gérer le temps
	NzClock secondClock, updateClock;

	// Ainsi qu'un compteur de FPS improvisé
	unsigned int fps = 0;

	// Début de la boucle de rendu du programme
	while (window.IsOpen())
	{
		// Ensuite nous allons traiter les évènements (Étape indispensable pour la fenêtre)
		NzEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case nzEventType_MouseMoved: // La souris a bougé
				{
					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.8f; // Sensibilité du déplacement

					// On modifie l'angle de la caméra grâce au déplacement relatif de la souris
					camAngles.yaw = NzNormalizeAngle(camAngles.yaw - event.mouseMove.deltaX*sensitivity);

					// Pour éviter les loopings mais surtout les problèmes de calculation de la matrice de vue, on restreint les angles
					camAngles.pitch = NzClamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					// On applique l'angle d'euler à la caméra
					camera.SetRotation(camAngles);

					// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenêtre
					// La fonction est codée de sorte qu'elle ne provoquera pas d'évènement MouseMoved
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_Quit: // L'utilisateur a cliqué sur la croix, ou l'OS veut terminer notre programme
					window.Close(); // On demande la fermeture de la fenêtre (Qui aura lieu au prochain tour de boucle)
					break;

				case nzEventType_KeyPressed: // Une touche a été pressée !
					if (event.key.code == NzKeyboard::Key::Escape)
						window.Close();
					break;

				default:
					break;
			}
		}

		// Mise à jour (Caméra)
		if (updateClock.GetMilliseconds() >= 1000/60) // 60 fois par seconde
		{
			// Gestion de la caméra free-fly (Déplacement)
			float cameraSpeed = 300.f; // Unités par seconde
			float elapsedTime = updateClock.GetSeconds();

			// Move agit par défaut dans l'espace local de la caméra, autrement dit la rotation est prise en compte

			// Si la flèche du haut ou la touche Z (vive ZQSD) est pressée, on avance
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Up) || NzKeyboard::IsKeyPressed(NzKeyboard::Z))
				camera.Move(NzVector3f::Forward() * cameraSpeed * elapsedTime);

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Down) || NzKeyboard::IsKeyPressed(NzKeyboard::S))
				camera.Move(NzVector3f::Forward() * -cameraSpeed * elapsedTime);

			// Etc...
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Left) || NzKeyboard::IsKeyPressed(NzKeyboard::Q))
				camera.Move(NzVector3f::Left() * cameraSpeed * elapsedTime);

			// Etc...
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Right) || NzKeyboard::IsKeyPressed(NzKeyboard::D))
				camera.Move(NzVector3f::Left() * -cameraSpeed * elapsedTime);

			// Majuscule pour monter, mais dans l'espace global (Sans tenir compte de la rotation)
			if (NzKeyboard::IsKeyPressed(NzKeyboard::LShift) || NzKeyboard::IsKeyPressed(NzKeyboard::RShift))
				camera.Move(NzVector3f::Up() * cameraSpeed * elapsedTime, nzCoordSys_Global);

			// Contrôle (Gauche ou droite) pour descendre, etc...
			if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl) || NzKeyboard::IsKeyPressed(NzKeyboard::RControl))
				camera.Move(NzVector3f::Up() * -cameraSpeed * elapsedTime, nzCoordSys_Global);

			// On relance l'horloge
			updateClock.Restart();
		}

		// Rendu de la scène

		// On active la caméra (Qui s'occupera de préparer la fenêtre au rendu)
		camera.Activate();

		// On vide le buffer de couleur et de profondeur pour réinitialiser le rendu
		// À l'avenir, ceci sera fait automatiquement par la scène via les backgrounds
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

		// On procède maintenant au rendu de la scène en elle-même, celui-ci se décompose en quatre étapes distinctes

		// Pour commencer, on mets à jour la scène, ceci appelle la méthode Update de tous les SceneNode enregistrés
		// pour la mise à jour globale (Scene::RegisterForUpdate)
		scene.Update();

		// Ensuite il y a le calcul de visibilité, la scène se sert de la caméra active pour effectuer un test de visibilité
		// afin de faire une liste des SceneNode visibles (Ex: Frustum culling)
		scene.Cull();

		// Ensuite il y a la mise à jour des SceneNode enregistrés pour la mise à jour visible (Exemple: Terrain)
		scene.UpdateVisible();

		// Pour terminer, il y a l'affichage en lui-même, de façon organisée et optimisée (Batching)
		scene.Draw();

		// Après avoir dessiné sur la fenêtre, il faut s'assurer qu'elle affiche cela
		// Cet appel ne fait rien d'autre qu'échanger les buffers de rendu (Double Buffering)
		window.Display();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (secondClock.GetMilliseconds() >= 1000) // Toutes les secondes
		{
			// On compte le nombre de Dr. Freak qui sont affichés actuellement
			unsigned int visibleNode = 0;
			for (NzModel& model : models)
			{
				if (model.IsVisible())
					visibleNode++;
			}

			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + NzString::Number(fps) + " FPS - " + NzString::Number(visibleNode) + u8" mod\u00E8les visibles");
			/*
			Note: En C++11 il est possible d'insérer de l'Unicode de façon standard,
			via quelque chose de similaire à u8"Cha\u00CEne de caract\u00E8res"
			Cependant, si le code source est encodé en UTF-8, cela fonctionnera aussi comme ceci : "Chaîne de caractères"
			*/

			// Et on réinitialise le compteur de FPS
			fps = 0;

			// Et on relance l'horloge pour refaire ça dans une seconde
			secondClock.Restart();
		}
	}

    return EXIT_SUCCESS;
}
