/*
* FirstScene - Première scène graphique
* Prérequis: Aucun
* Utilisation du module utilitaire et graphique
** Présente :
	* -Création et gestion d'une fenêtre (Traitement des évènements clavier/souris)
	* -Gestion du clavier(Récupération de l'état d'une touche)
	* -Des outils pour afficher une scène basique via le chargement d'un modèle (et son affichage)
	* -Éclairage directionnel
	* -Gestion d'une caméra free-fly (Avec déplacement fluide)
	* -Gestion basique d'une horloge
	* -Console
*/

#include <Nazara/Core/Clock.hpp> // Horloges
#include <Nazara/Lua.hpp> // Module de scripting
#include <Nazara/Graphics.hpp> // Module graphique
#include <Nazara/Renderer.hpp> // Module de rendu
#include <Nazara/Utility.hpp> // Module utilitaire
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/Console.hpp>
#include <NDK/Systems.hpp>
#include <NDK/LuaAPI.hpp>
#include <NDK/Sdk.hpp>
#include <NDK/World.hpp>
#include <iostream>

// Petite fonction permettant de rendre le déplacement de la caméra moins ridige
Nz::Vector3f DampedString(const Nz::Vector3f& currentPos, const Nz::Vector3f& targetPos, float frametime, float springStrength = 3.f);

int main()
{
	// Ndk::Application est une classe s'occupant de l'initialisation du moteur ainsi que de la gestion de beaucoup de choses
	Ndk::Application application;

	// Nazara étant initialisé, nous pouvons créer le monde pour contenir notre scène.
	// Dans un ECS, le monde représente bien ce que son nom indique, c'est l'ensemble de ce qui existe au niveau de l'application.
	// Il contient les systèmes et les entités, ces dernières contiennent les composants.
	// Il est possible d'utiliser plusieurs mondes au sein d'une même application, par exemple pour gérer un mélange de 2D et de 3D,
	// mais nous verrons cela dans un prochain exemple.
	Ndk::WorldHandle world = application.AddWorld().CreateHandle();

	// Nous pouvons maintenant ajouter des systèmes, mais dans cet exemple nous nous contenterons de ceux de base.

	// La première chose que nous faisons est d'ajouter un background (fond) à notre scène.
	// Il en existe plusieurs types, le moteur inclut pour l'instant trois d'entre eux:
	// -ColorBackground: Une couleur unie en fond
	// -SkyboxBackground: Une skybox en fond, un cube à six faces rendu autour de la caméra (En perdant la notion de distance)
	// -TextureBackground: Une texture en fond, celle-ci sera affichée derrière la scène

	// Nous choisirons ici une Skybox, cette dernière étant l'effet le plus réussi et convenant très bien à une scène spatiale
	// Pour commencer il faut charger une texture de type cubemap, certaines images sont assemblées de cette façon,
	// comme celle que nous allons utiliser.
	// En réalité les textures "cubemap" regroupent six faces en une, pour faciliter leur utilisation.

	// Nous créons une nouvelle texture et prenons une référence sur celle-ci (à la manière des pointeurs intelligents)
	Nz::TextureRef texture = Nz::Texture::New();
	if (texture->LoadCubemapFromFile("resources/skybox-space.png"))
	{
		// Si la création du cubemap a fonctionné

		// Nous créons alors le background à partir de notre texture (celui-ci va référencer notre texture, notre pointeur ne sert alors plus à rien).
		Nz::SkyboxBackgroundRef skybox = Nz::SkyboxBackground::New(std::move(texture));

		// Accédons maintenant au système de rendu faisant partie du monde
		Ndk::RenderSystem& renderSystem = world->GetSystem<Ndk::RenderSystem>(); // Une assertion valide la précondition "le système doit faire partie du monde"

		// Nous assignons ensuite notre skybox comme "fond par défaut" du système
		// La notion "par défaut" existe parce qu'une caméra pourrait utiliser son propre fond lors du rendu,
		// le fond par défaut est utilisé lorsque la caméra n'a pas de fond propre assigné
		renderSystem.SetDefaultBackground(std::move(skybox));

		// Notre skybox est maintenant référencée par le système, lui-même appartenant au monde, aucune libération explicite n'est nécessaire
	}
	else
		// Le chargement a échoué
		std::cout << "Failed to load skybox" << std::endl;

	// Ensuite, nous allons rajouter un modèle à notre scène.

	// Les modèles représentent, globalement, tout ce qui est visible en trois dimensions.
	// Nous choisirons ici un vaisseau spatial (Quoi de mieux pour une scène spatiale ?)

	// Encore une fois, nous récupérons une référence plutôt que l'objet lui-même (cela va être très utile par la suite)
	Nz::ModelRef spaceshipModel = Nz::Model::New();

	// Nous allons charger notre modèle depuis un fichier, mais nous pouvons ajuster le modèle lors du chargement via
	// une structure permettant de paramétrer le chargement des modèles
	Nz::ModelParameters params;

	// Le format OBJ ne précise aucune échelle pour ses données, contrairement à Nazara (une unité = un mètre en 3D).
	// Comme le vaisseau est très grand (Des centaines de mètres de long), nous allons le rendre plus petit pour les besoins de la démo.
	// Ce paramètre sert à indiquer la mise à l'échelle désirée lors du chargement du modèle.
	params.mesh.matrix.MakeScale(Nz::Vector3f(0.01f)); // Un centième de la taille originelle

	// Les UVs de ce fichier sont retournées (repère OpenGL, origine coin bas-gauche) par rapport à ce que le moteur attend (haut-gauche)
	// Nous devons donc indiquer au moteur de les retourner lors du chargement
	params.mesh.texCoordScale.Set(1.f, -1.f);

	// Nazara va par défaut optimiser les modèles pour un rendu plus rapide, cela peut prendre du temps et n'est pas nécessaire ici
	params.mesh.optimizeIndexBuffers = false;

	// On charge ensuite le modèle depuis son fichier
	// Le moteur va charger le fichier et essayer de retrouver les fichiers associés (comme les matériaux, textures, ...)
	if (!spaceshipModel->LoadFromFile("resources/Spaceship/spaceship.obj", params))
	{
		// Si le chargement a échoué (fichier inexistant/invalide), il ne sert à rien de continuer
		std::cout << "Failed to load spaceship" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// Nous voulons afficher quelques statistiques relatives au modèle, comme le nombre de sommets et de triangles
	// Pour cela, nous devons accéder au mesh (maillage 3D)

	// Note: Si nous voulions stocker le mesh pour nous en servir après, nous devrions alors récupérer une référence pour nous assurer
	// qu'il ne sera pas supprimé tant que nous l'utilisons, mais ici nous faisons un accès direct et ne nous servons plus du pointeur par la suite
	// Il est donc acceptable d'utiliser un pointeur nu ici.
	Nz::Mesh* mesh = spaceshipModel->GetMesh();
	std::cout << mesh->GetVertexCount() << " sommets" << std::endl;
	std::cout << mesh->GetTriangleCount() << " triangles" << std::endl;

	// En revanche, le format OBJ ne précise pas l'utilisation d'une normal map, nous devons donc la charger manuellement
	// Pour commencer on récupère le matériau du mesh, celui-ci en possède plusieurs mais celui qui nous intéresse,
	// celui de la coque, est le second (Cela est bien entendu lié au modèle en lui-même)
	Nz::Material* material = spaceshipModel->GetMaterial(1); // Encore une fois nous ne faisons qu'un accès direct.

	// On lui indique ensuite le chemin vers la normal map
	if (!material->SetNormalMap("resources/Spaceship/Texture/normal.png"))
	{
		// Le chargement a échoué, peut-être le fichier n'existe pas, ou n'est pas reconnu par le moteur
		// Mais ce n'est pas une erreur critique, le rendu peut quand même se faire (Mais sera moins détaillé)
		std::cout << "Failed to load normal map" << std::endl;
	}

	// Bien, nous avons un modèle valide, mais celui-ci ne consiste qu'en des informations de rendu, de matériaux et de textures.
	// Commençons donc par créer une entité vide, cela se fait en demandant au monde de générer une nouvelle entité.
	Ndk::EntityHandle spaceship = world->CreateEntity();

	// Note: Nous ne récupérons pas l'entité directement mais un "handle" vers elle, ce dernier est un pointeur intelligent non-propriétaire.
	// Pour des raisons techniques, le pointeur de l'entité peut venir à changer, ou l'entité être simplement détruite pour n'importe quelle raison.
	// Le Handle nous permet de maintenir un pointeur valide vers notre entité, et invalidé automatiquement à sa mort.

	// Nous avons désormais une entité, mais celle-ci ne contient rien et n'a d'autre propriété qu'un identifiant
	// Nous devons donc lui rajouter les composants que nous voulons.

	// Un NodeComponent donne à notre entité une position, rotation, échelle, et nous permet de l'attacher à d'autres entités (ce que nous ne ferons pas ici).
	// Étant donné que par défaut, un NodeComponent se place en (0,0,0) sans rotation et avec une échelle de 1,1,1 et que cela nous convient,
	// nous n'avons pas besoin d'agir sur le composant créé.
	spaceship->AddComponent<Ndk::NodeComponent>();
	//spaceship->AddComponent<Ndk::VelocityComponent>().linearVelocity.Set(-1.f, 0.f, 0.f);

	// Bien, notre entité nouvellement créé dispose maintenant d'une position dans la scène, mais est toujours invisible
	// Nous lui ajoutons donc un GraphicsComponent
	Ndk::GraphicsComponent& spaceshipGraphics = spaceship->AddComponent<Ndk::GraphicsComponent>();

	// Ce composant sert de point d'attache pour tous les renderables instanciés (tels que les modèles, les sprites, le texte, etc.)
	// Cela signifie également qu'un modèle peut être attaché à autant d'entités que nécessaire.
	// Note: Afin de maximiser les performances, essayez d'avoir le moins de renderables instanciés/matériaux et autres ressources possible
	// le moteur fonctionne selon le batching et regroupera par exemple tous les modèles identiques ensembles lors du rendu.
	spaceshipGraphics.Attach(spaceshipModel);

	// Nous avons besoin également d'une caméra pour servir de point de vue à notre scène, celle-ci sera à l'écart du modèle
	// regardant dans sa direction.

	// On conserve la rotation à part via des angles d'eulers pour la caméra free-fly
	Nz::EulerAnglesf camAngles(0.f, -20.f, 0.f);

	// Nous créons donc une seconde entité
	// Note: La création d'entité est une opération légère au sein du moteur, mais plus vous aurez d'entités et plus le processeur devra travailler.
	Ndk::EntityHandle camera = world->CreateEntity();

	// Notre caméra a elle aussi besoin d'être positionnée dans la scène
	Ndk::NodeComponent& cameraNode = camera->AddComponent<Ndk::NodeComponent>();
	cameraNode.SetPosition(0.f, 0.25f, 2.f); // On place la caméra à l'écart
	cameraNode.SetRotation(camAngles);

	// Et dispose d'un composant pour chaque point de vue de la scène, le CameraComponent
	Ndk::CameraComponent& cameraComp = camera->AddComponent<Ndk::CameraComponent>();

	// Ajoutons un composant écouteur, si nous venions à avoir du son
	camera->AddComponent<Ndk::ListenerComponent>();

	// Et on n'oublie pas de définir les plans délimitant le champs de vision
	// (Seul ce qui se trouvera entre les deux plans sera rendu)

	// La distance entre l'oeil et le plan éloigné
	cameraComp.SetZFar(5000.f);

	// La distance entre l'oeil et le plan rapproché (0 est une valeur interdite car la division par zéro l'est également)
	cameraComp.SetZNear(0.1f);

	// Attention que le ratio entre les deux (zFar/zNear) doit rester raisonnable, dans le cas contraire vous risquez un phénomène
	// de "Z-Fighting" (Impossibilité de déduire quelle surface devrait apparaître en premier) sur les surfaces éloignées.

	// Il ne nous manque plus maintenant que de l'éclairage, sans quoi la scène sera complètement noire
	// Il existe trois types de lumières:
	// -DirectionalLight: Lumière infinie sans position, envoyant de la lumière dans une direction particulière
	// -PointLight: Lumière située à un endroit précis, envoyant de la lumière finie dans toutes les directions
	// -SpotLight: Lumière située à un endroit précis, envoyant de la lumière vers un endroit donné, avec un angle de diffusion

	// Nous allons créer une lumière directionnelle pour représenter la nébuleuse de notre skybox
	// Encore une fois, nous créons notre entité
	Ndk::EntityHandle nebulaLight = world->CreateEntity();

	// Lui ajoutons une position dans la scène
	Ndk::NodeComponent& nebulaLightNode = nebulaLight->AddComponent<Ndk::NodeComponent>();

	// Et ensuite le composant principal, le LightComponent
	Ndk::LightComponent& nebulaLightComp = nebulaLight->AddComponent<Ndk::LightComponent>(Nz::LightType_Directional);

	// Il nous faut ensuite configurer la lumière
	// Pour commencer, sa couleur, la nébuleuse étant d'une couleur jaune, j'ai choisi ces valeurs
	nebulaLightComp.SetColor(Nz::Color(255, 182, 90));

	// Nous appliquons ensuite une rotation de sorte que la lumière dans la même direction que la nébuleuse
	nebulaLightNode.SetRotation(Nz::EulerAnglesf(0.f, 102.f, 0.f));

	// Nous allons maintenant créer la fenêtre, dans laquelle nous ferons nos rendus
	// Celle-ci demande des paramètres plus complexes

	// Pour commencer le mode vidéo, celui-ci va définir la taille de la zone de rendu et le nombre de bits par pixels
	Nz::VideoMode mode = Nz::VideoMode::GetDesktopMode(); // Nous récupérons le mode vidéo du bureau

	// Nous allons prendre les trois quarts de la résolution du bureau pour notre fenêtre
	mode.width = 3 * mode.width / 4;
	mode.height = 3 * mode.height / 4;

	// Maintenant le titre, rien de plus simple...
	Nz::String windowTitle = "Nazara Demo - First scene";

	// Ensuite, le "style" de la fenêtre, possède-t-elle des bordures, peut-on cliquer sur la croix de fermeture,
	// peut-on la redimensionner, ...
	Nz::WindowStyleFlags style = Nz::WindowStyle_Default; // Nous prenons le style par défaut, autorisant tout ce que je viens de citer

	// Ensuite, les paramètres du contexte de rendu
	// On peut configurer le niveau d'antialiasing, le nombre de bits du depth buffer et le nombre de bits du stencil buffer
	// Nous désirons avoir un peu d'antialiasing (4x), les valeurs par défaut pour le reste nous conviendrons très bien
	Nz::RenderTargetParameters parameters;
	parameters.antialiasingLevel = 4;

	Nz::RenderWindow& window = application.AddWindow<Nz::RenderWindow>(mode, windowTitle, style, parameters);
	if (!window.IsValid())
	{
		std::cout << "Failed to create render window" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// On fait disparaître le curseur de la souris
	window.SetCursor(Nz::SystemCursor_None);

	// On lie la caméra à la fenêtre
	cameraComp.SetTarget(&window);

	// Et on créé une horloge pour gérer le temps
	Nz::Clock updateClock;
	Nz::UInt64 updateAccumulator = 0;

	// Quelques variables de plus pour notre caméra
	bool smoothMovement = true;
	Nz::Vector3f targetPos = cameraNode.GetPosition();

	window.EnableEventPolling(true); // Déprécié

	application.EnableConsole(true);
	application.EnableFPSCounter(true);

	Ndk::Application::ConsoleOverlay& consoleOverlay = application.GetConsoleOverlay();
	consoleOverlay.lua.PushGlobal("Spaceship", spaceship->CreateHandle());
	consoleOverlay.lua.PushGlobal("World", world->CreateHandle());

	// Début de la boucle de rendu du programme (s'occupant par exemple de mettre à jour le monde)
	while (application.Run())
	{
		// Ensuite nous allons traiter les évènements (Étape indispensable pour la fenêtre)
		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType_MouseMoved: // La souris a bougé
				{
					if (application.IsConsoleEnabled())
					{
						Ndk::Application::ConsoleOverlay& consoleOverlay = application.GetConsoleOverlay();
						if (consoleOverlay.console->IsVisible())
							break;
					}

					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = Nz::NormalizeAngle(camAngles.yaw - event.mouseMove.deltaX*sensitivity);

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					// On applique les angles d'Euler à notre caméra
					cameraNode.SetRotation(camAngles);

					// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenêtre
					// Cette fonction est codée de sorte à ne pas provoquer d'évènement MouseMoved
					Nz::Mouse::SetPosition(window.GetWidth() / 2, window.GetHeight() / 2, window);
					break;
				}

				case  Nz::WindowEventType_Quit: // L'utilisateur a cliqué sur la croix, ou l'OS veut terminer notre programme
					application.Quit();
					break;

				case Nz::WindowEventType_KeyPressed: // Une touche a été pressée !
					if (event.key.code == Nz::Keyboard::Key::Escape)
						window.Close();
					else if (event.key.code == Nz::Keyboard::F1)
					{
						if (smoothMovement)
						{
							targetPos = cameraNode.GetPosition();
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

		Nz::UInt64 elapsedUS = updateClock.GetMicroseconds();
		// On relance l'horloge
		updateClock.Restart();

		// Mise à jour (Caméra)
		const Nz::UInt64 updateRate = 1000000 / 60; // 60 fois par seconde
		updateAccumulator += elapsedUS;

		if (updateAccumulator >= updateRate)
		{
			// Le temps écoulé en seconde depuis la dernière fois que ce bloc a été exécuté
			float elapsedTime = updateAccumulator / 1000000.f;

			// Vitesse de déplacement de la caméra
			float cameraSpeed = 3.f * elapsedTime; // Trois mètres par seconde

			bool move = true;

			if (application.IsConsoleEnabled())
			{
				Ndk::Application::ConsoleOverlay& consoleOverlay = application.GetConsoleOverlay();
				if (consoleOverlay.console->IsVisible())
					move = false;
			}

			if (move)
			{
				// Si la touche espace est enfoncée, notre vitesse de déplacement est multipliée par deux
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Space))
					cameraSpeed *= 2.f;

				// Pour que nos déplacement soient liés à la rotation de la caméra, nous allons utiliser
				// les directions locales de la caméra

				// Si la flèche du haut ou la touche Z (vive ZQSD) est pressée, on avance
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Z))
					targetPos += cameraNode.GetForward() * cameraSpeed;

				// Si la flèche du bas ou la touche S est pressée, on recule
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::S))
					targetPos += cameraNode.GetBackward() * cameraSpeed;

				// Etc...
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Q))
					targetPos += cameraNode.GetLeft() * cameraSpeed;

				// Etc...
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::D))
					targetPos += cameraNode.GetRight() * cameraSpeed;

				// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::RShift))
					targetPos += Nz::Vector3f::Up() * cameraSpeed;

				// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::RControl))
					targetPos += Nz::Vector3f::Down() * cameraSpeed;
			}

			cameraNode.SetPosition((smoothMovement) ? DampedString(cameraNode.GetPosition(), targetPos, elapsedTime) : targetPos, Nz::CoordSys_Global);
			updateAccumulator = 0;
		}

		// Après avoir dessiné sur la fenêtre, il faut s'assurer qu'elle affiche cela
		// Cet appel ne fait rien d'autre qu'échanger les buffers de rendu (Double Buffering)
		window.Display();
	}

	return EXIT_SUCCESS;
}

Nz::Vector3f DampedString(const Nz::Vector3f& currentPos, const Nz::Vector3f& targetPos, float frametime, float springStrength)
{
	// Je ne suis pas l'auteur de cette fonction
	// Je l'ai reprise du programme "Floaty Camera Example" et adaptée au C++
	// Trouvé ici: http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/opengl_programming.html#4
	// Tout le mérite revient à l'auteur (Qui me permettra ainsi d'améliorer les démos, voire même le moteur)

	// calculate the displacement between the target and the current position
	Nz::Vector3f displacement = targetPos - currentPos;

	// whats the distance between them?
	float displacementLength = displacement.GetLength();

	// Stops small position fluctuations (integration errors probably - since only using euler)
	if (Nz::NumberEquals(displacementLength, 0.f))
		return currentPos;

	float invDisplacementLength = 1.f / displacementLength;

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
