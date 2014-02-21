/*
** FirstScene - Première scène graphique
** Prérequis: Aucun
** Utilisation du module utilitaire et graphique
** Présente:
** - Création et gestion d'une fenêtre (Traitement des évènements clavier/souris)
** - Gestion du clavier (Récupération de l'état d'une touche)
** - Des outils pour afficher une scène basique via le chargement d'un modèle (et son affichage)
** - Éclairage directionnel
** - Gestion d'une caméra free-fly (Avec déplacement fluide)
** - Gestion basique d'une horloge
*/

#include <Nazara/Core/Clock.hpp> // Horloges
#include <Nazara/Graphics.hpp> // Module graphique
#include <Nazara/Renderer.hpp> // Module de rendu
#include <Nazara/Utility.hpp> // Module utilitaire
#include <iostream>

// Petite fonction permettant de rendre le déplacement de la caméra moins ridige
NzVector3f DampedString(const NzVector3f& currentPos, const NzVector3f& targetPos, float frametime, float springStrength = 3.f);

int main()
{
	// Pour commencer, nous initialisons le module Graphique, celui-ci va provoquer l'initialisation (dans l'ordre),
	// du noyau (Core), Utility, Renderer.
	// NzInitializer est une classe RAII appelant Initialize dans son constructeur et Uninitialize dans son destructeur.
	// Autrement dit, une fois ceci fait nous n'avons plus à nous soucier de la libération du moteur.
	NzInitializer<NzGraphics> nazara;
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
	// Par exemple, une pièce contenant une télévision, laquelle affichant des images provenant d'une Camera
	// Le rendu sera alors plus efficace en créant deux scènes, une pour la pièce et l'autre pour les images de la télé.
	// Cela diminuera le nombre de SceneNode à gérer pour chaque scène, et vous permettra même de ne pas afficher la scène
	// affichée dans la télé si cette dernière n'est pas visible dans la première scène.
	NzScene scene;

	// La première chose que nous faisons est d'ajouter un background (fond) à la scène.
	// Il en existe plusieurs types, le moteur inclut pour l'instant trois d'entre eux:
	// -ColorBackground: Une couleur unie en fond
	// -SkyboxBackground: Une skybox en fond, un cube à six faces rendu autour de la caméra (En perdant la notion de distance)
	// -TextureBackground: Une texture en fond, celle-ci sera affichée derrière la scène

	// Nous choisirons ici une Skybox, cette dernière étant l'effet le plus réussi et convenant très bien à une scène spatiale
	// Pour commencer il faut charger une texture de type cubemap, certaines images sont assemblées de cette façon,
	// comme celle que nous allons utiliser.
	// En réalité les textures "cubemap" regroupent six faces en une, pour faciliter leur utilisation.
	NzTexture* texture = new NzTexture;
	if (texture->LoadCubemapFromFile("resources/skybox-space.png"))
	{
		// Si la création du cubemap a fonctionné

		// Nous indiquons que la texture est "non-persistente", autrement dit elle sera libérée automatiquement par le moteur
		// à l'instant précis où elle ne sera plus utilisée, dans ce cas-ci, ce sera à la libération de l'objet skybox,
		// ceci arrivant lorsqu'un autre background est affecté à la scène, ou lorsque la scène sera libérée
		texture->SetPersistent(false);

		// Nous créons le background en lui affectant la texture
		NzSkyboxBackground* background = new NzSkyboxBackground(texture);

		// Nous pouvons en profiter pour paramétrer le background.
		// Cependant, nous n'avons rien de spécial à faire ici, nous pouvons donc l'envoyer à la scène.
		scene.SetBackground(background);

		// Comme indiqué plus haut, la scène s'occupera automatiquement de la libération de notre background
	}
	else
	{
		delete texture; // Le chargement a échoué, nous libérons la texture
		std::cout << "Failed to load skybox" << std::endl;
	}

	// Ensuite, nous allons rajouter un modèle à notre scène.
	// Les modèles représentent, globalement, tout ce qui est visible en trois dimensions.
	// Nous choisirons ici un vaisseau spatial (Quoi de mieux pour une scène spatiale ?)
	NzModel spaceship;

	// Une structure permettant de paramétrer le chargement des modèles
	NzModelParameters params;

	// Le format OBJ ne précise aucune échelle pour ses données, contrairement à Nazara (une unité = un mètre).
	// Comme le vaisseau est très grand (Des centaines de mètres de long), nous allons le rendre plus petit
	// pour les besoins de la démo.
	// Ce paramètre sert à indiquer la mise à l'échelle désirée lors du chargement du modèle.
	params.mesh.scale.Set(0.01f); // Un centième de la taille originelle

	// On charge ensuite le modèle depuis son fichier
	// Le moteur va charger le fichier et essayer de retrouver les fichiers associés (comme les matériaux, textures, ...)
	if (!spaceship.LoadFromFile("resources/Spaceship/spaceship.obj", params))
	{
		std::cout << "Failed to load spaceship" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// Nous voulons afficher quelques statistiques relatives au modèle, comme le nombre de sommets et de triangles
	// Pour cela, nous devons accéder au mesh (maillage 3D)
	NzMesh* mesh = spaceship.GetMesh();

	std::cout << mesh->GetVertexCount() << " sommets" << std::endl;
	std::cout << mesh->GetTriangleCount() << " triangles" << std::endl;

	// En revanche, le format OBJ ne précise pas l'utilisation d'une normal map, nous devons donc la charger manuellement
	// Pour commencer on récupère le matériau du mesh, celui-ci en possède plusieurs mais celui qui nous intéresse,
	// celui de la coque, est le second (Cela est bien entendu lié au modèle en lui-même)
	NzMaterial* material = spaceship.GetMaterial(1);

	// On lui indique ensuite le chemin vers la normal map
	if (!material->SetNormalMap("resources/Spaceship/Texture/normal.png"))
	{
		// Le chargement a échoué, peut-être le fichier n'existe pas, ou n'est pas reconnu par le moteur
		// Mais ce n'est pas une erreur critique, le rendu peut quand même se faire (Mais sera moins détaillé)
		std::cout << "Failed to load normal map" << std::endl;
	}

	// Il nous reste à attacher le modèle à la scène, ce qui se fait simplement via cet appel
	spaceship.SetParent(scene);
	// Et voilà, à partir de maintenant le modèle fait partie de la hiérarchie de la scène, et sera donc rendu avec cette dernière

	// Nous avons besoin également d'une caméra, pour des raisons évidentes, celle-ci sera à l'écart du modèle
	// regardant dans sa direction.

	// On conserve la rotation à part via des angles d'eulers pour la caméra free-fly
	NzEulerAnglesf camAngles(0.f, -20.f, 0.f);

	NzCamera camera;
	camera.SetPosition(0.f, 0.25f, 2.f); // On place la caméra à l'écart
	camera.SetRotation(camAngles);

	// Et on n'oublie pas de définir les plans délimitant le champs de vision
	// (Seul ce qui se trouvera entre les deux plans sera rendu)

	// La distance entre l'oeil et le plan éloigné
	camera.SetZFar(5000.f);

	// La distance entre l'oeil et le plan rapproché (0 est une valeur interdite car la division par zéro l'est également)
	camera.SetZNear(0.1f);

	// On indique à la scène que le viewer (Le point de vue) sera la caméra
	scene.SetViewer(camera);

	// Attention que le ratio entre les deux (zFar/zNear) doit rester raisonnable, dans le cas contraire vous risquez un phénomène
	// de "Z-Fighting" (Impossibilité de déduire quelle surface devrait apparaître en premier) sur les surfaces éloignées.

	// Il ne nous manque plus maintenant que de l'éclairage, sans quoi la scène sera complètement noire
	// Il existe trois types de lumières:
	// -DirectionalLight: Lumière infinie sans position, envoyant de la lumière dans une direction particulière
	// -PointLight: Lumière située à un endroit précis, envoyant de la lumière finie dans toutes les directions
	// -SpotLight: Lumière située à un endroit précis, envoyant de la lumière vers un endroit donné, avec un angle de diffusion

	// Nous choisissons une lumière directionnelle représentant la nébuleuse de notre skybox
	NzLight nebulaLight(nzLightType_Directional);

	// Il nous faut ensuite configurer la lumière
	// Pour commencer, sa couleur, la nébuleuse étant d'une couleur jaune, j'ai choisi ces valeurs
	nebulaLight.SetColor(NzColor(255, 182, 90));

	// Nous appliquons ensuite une rotation de sorte que la lumière dans la même direction que la nébuleuse
	nebulaLight.SetRotation(NzEulerAnglesf(0.f, 102.f, 0.f));

	// Et nous ajoutons la lumière à la scène
	nebulaLight.SetParent(scene);

	// Nous allons maintenant créer la fenêtre, dans laquelle nous ferons nos rendus
	// Celle-ci demande des paramètres plus complexes

	// Pour commencer le mode vidéo, celui-ci va définir la taille de la zone de rendu et le nombre de bits par pixels
	NzVideoMode mode = NzVideoMode::GetDesktopMode(); // Nous récupérons le mode vidéo du bureau

	// Nous allons prendre les trois quarts de la résolution du bureau pour notre fenêtre
	mode.width *= 3.f/4.f;
	mode.height *= 3.f/4.f;

	// Maintenant le titre, rien de plus simple...
	NzString windowTitle = "Nazara Demo - First scene";

	// Ensuite, le "style" de la fenêtre, possède-t-elle des bordures, peut-on cliquer sur la croix de fermeture,
	// peut-on la redimensionner, ...
	nzWindowStyleFlags style = nzWindowStyle_Default; // Nous prenons le style par défaut, autorisant tout ce que je viens de citer

	// Ensuite, les paramètres du contexte de rendu
	// On peut configurer le niveau d'antialiasing, le nombre de bits du depth buffer et le nombre de bits du stencil buffer
	// Nous désirons avoir un peu d'antialiasing (4x), les valeurs par défaut pour le reste nous conviendrons très bien
	NzRenderTargetParameters parameters;
	parameters.antialiasingLevel = 4;

	NzRenderWindow window(mode, windowTitle, style, parameters);
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

	// Quelques variables de plus pour notre caméra
	bool smoothMovement = true;
	NzVector3f targetPos = camera.GetPosition();

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
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = NzNormalizeAngle(camAngles.yaw - event.mouseMove.deltaX*sensitivity);

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = NzClamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					// On applique les angles d'Euler à notre caméra
					camera.SetRotation(camAngles);

					// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenêtre
					// Cette fonction est codée de sorte à ne pas provoquer d'évènement MouseMoved
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_Quit: // L'utilisateur a cliqué sur la croix, ou l'OS veut terminer notre programme
					window.Close(); // On demande la fermeture de la fenêtre (Qui aura lieu au prochain tour de boucle)
					break;

				case nzEventType_KeyPressed: // Une touche a été pressée !
					if (event.key.code == NzKeyboard::Key::Escape)
						window.Close();
					else if (event.key.code == NzKeyboard::F1)
					{
						if (smoothMovement)
						{
							targetPos = camera.GetPosition();
							smoothMovement = false;
						}
						else
							smoothMovement = true;
					}
					break;

				default:
					break;
			}
		}

		// Mise à jour (Caméra)
		if (updateClock.GetMilliseconds() >= 1000/60) // 60 fois par seconde
		{
			// Le temps écoulé depuis la dernière fois que ce bloc a été exécuté
			float elapsedTime = updateClock.GetSeconds();

			// Vitesse de déplacement de la caméra
			float cameraSpeed = 3.f * elapsedTime; // Trois mètres par seconde

			// Si la touche espace est enfoncée, notre vitesse de déplacement est multipliée par deux
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
				cameraSpeed *= 2.f;

			// Pour que nos déplacement soient liés à la rotation de la caméra, nous allons utiliser
			// les directions locales de la caméra

			// Si la flèche du haut ou la touche Z (vive ZQSD) est pressée, on avance
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Up) || NzKeyboard::IsKeyPressed(NzKeyboard::Z))
				targetPos += camera.GetForward() * cameraSpeed;

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Down) || NzKeyboard::IsKeyPressed(NzKeyboard::S))
				targetPos += camera.GetBackward() * cameraSpeed;

			// Etc...
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Left) || NzKeyboard::IsKeyPressed(NzKeyboard::Q))
				targetPos += camera.GetLeft() * cameraSpeed;

			// Etc...
			if (NzKeyboard::IsKeyPressed(NzKeyboard::Right) || NzKeyboard::IsKeyPressed(NzKeyboard::D))
				targetPos += camera.GetRight() * cameraSpeed;

			// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
			if (NzKeyboard::IsKeyPressed(NzKeyboard::LShift) || NzKeyboard::IsKeyPressed(NzKeyboard::RShift))
				targetPos += NzVector3f::Up() * cameraSpeed;

			// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
			if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl) || NzKeyboard::IsKeyPressed(NzKeyboard::RControl))
				targetPos += NzVector3f::Down() * cameraSpeed;

			camera.SetPosition((smoothMovement) ? DampedString(camera.GetPosition(), targetPos, elapsedTime) : targetPos, nzCoordSys_Global);

			// On relance l'horloge
			updateClock.Restart();
		}

		// Rendu de la scène:
		// On procède maintenant au rendu de la scène en elle-même, celui-ci se décompose en quatre étapes distinctes

		// Pour commencer, on met à jour la scène, ceci appelle la méthode Update de tous les SceneNode enregistrés
		// pour la mise à jour globale (Scene::RegisterForUpdate)
		scene.Update();

		// Ensuite il y a le calcul de visibilité, la scène se sert de la caméra active pour effectuer un test de visibilité
		// afin de faire une liste des SceneNode visibles (Ex: Frustum culling)
		scene.Cull();

		// Ensuite il y a la mise à jour des SceneNode enregistrés pour la mise à jour visible (Exemple: Terrain)
		scene.UpdateVisible();

		// Pour terminer, il y a l'affichage en lui-même, de façon organisée et optimisée (Batching)
		scene.Draw();

		NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
		NzDebugDrawer::Draw(spaceship.GetBoundingVolume().aabb);

		// Après avoir dessiné sur la fenêtre, il faut s'assurer qu'elle affiche cela
		// Cet appel ne fait rien d'autre qu'échanger les buffers de rendu (Double Buffering)
		window.Display();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (secondClock.GetMilliseconds() >= 1000) // Toutes les secondes
		{
			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + NzString::Number(fps) + " FPS");

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

NzVector3f DampedString(const NzVector3f& currentPos, const NzVector3f& targetPos, float frametime, float springStrength)
{
	// Je ne suis pas l'auteur de cette fonction
	// Je l'ai reprise du programme "Floaty Camera Example" et adaptée au C++
	// Trouvé ici: http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/opengl_programming.html#4
	// Tout le mérite revient à l'auteur (Qui me permettra ainsi d'améliorer les démos, voire même le moteur)

	// calculate the displacement between the target and the current position
	NzVector3f displacement = targetPos - currentPos;

	// whats the distance between them?
	float displacementLength = displacement.GetLength();

	// Stops small position fluctuations (integration errors probably - since only using euler)
	if (NzNumberEquals(displacementLength, 0.f))
		return currentPos;

	float invDisplacementLength = 1.f/displacementLength;

	const float dampConstant = 0.000065f; // Something v.small to offset 1/ displacement length

	// the strength of the spring increases the further away the camera is from the target.
	float springMagitude = springStrength*displacementLength + dampConstant*invDisplacementLength;

	// Normalise the displacement and scale by the spring magnitude
	// and the amount of time passed
	float scalar = std::min(invDisplacementLength * springMagitude * frametime, 1.f);
	displacement *= scalar;

	// move the camera a bit towards the target
	return currentPos + displacement;
}
