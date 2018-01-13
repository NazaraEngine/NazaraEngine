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
#include <Nazara/Network.hpp> // Module utilitaire
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

	// Nazara étant initialisé, nous pouvons créer le monde pour contenir notre scéne.
	// Dans un ECS, le monde représente bien ce que son nom indique, c'est l'ensemble de ce qui existe au niveau de l'application.
	// Il contient les systémes et les entités, ces derniéres contiennent les composants.
	// Il est possible d'utiliser plusieurs mondes au sein d'une méme application, par exemple pour gérer un mélange de 2D et de 3D,
	// mais nous verrons cela dans un prochain exemple.
	Ndk::WorldHandle world = application.AddWorld().CreateHandle();

	// Nous pouvons maintenant ajouter des systémes, mais dans cet exemple nous nous contenterons de ceux de base.

	// La premiére chose que nous faisons est d'ajouter un background (fond) é notre scéne.
	// Il en existe plusieurs types, le moteur inclut pour l'instant trois d'entre eux:
	// -ColorBackground: Une couleur unie en fond
	// -SkyboxBackground: Une skybox en fond, un cube é six faces rendu autour de la caméra (En perdant la notion de distance)
	// -TextureBackground: Une texture en fond, celle-ci sera affichée derriére la scéne

	// Nous choisirons ici une Skybox, cette derniére étant l'effet le plus réussi et convenant trés bien é une scéne spatiale
	// Pour commencer il faut charger une texture de type cubemap, certaines images sont assemblées de cette faéon,
	// comme celle que nous allons utiliser.
	// En réalité les textures "cubemap" regroupent six faces en une, pour faciliter leur utilisation.

	// Nous créons une nouvelle texture et prenons une référence sur celle-ci (é la maniére des pointeurs intelligents)
	Nz::TextureRef texture = Nz::Texture::New();
	if (texture->LoadCubemapFromFile("resources/skybox-space.png"))
	{
		// Si la création du cubemap a fonctionné

		// Nous créons alors le background é partir de notre texture (celui-ci va référencer notre texture, notre pointeur ne sert alors plus é rien).
		Nz::SkyboxBackgroundRef skybox = Nz::SkyboxBackground::New(std::move(texture));

		// Accédons maintenant au systéme de rendu faisant partie du monde
		Ndk::RenderSystem& renderSystem = world->GetSystem<Ndk::RenderSystem>(); // Une assertion valide la précondition "le systéme doit faire partie du monde"

																				 // Nous assignons ensuite notre skybox comme "fond par défaut" du systéme
																				 // La notion "par défaut" existe parce qu'une caméra pourrait utiliser son propre fond lors du rendu,
																				 // le fond par défaut est utilisé lorsque la caméra n'a pas de fond propre assigné
		renderSystem.SetDefaultBackground(std::move(skybox));

		// Notre skybox est maintenant référencée par le systéme, lui-méme appartenant au monde, aucune libération explicite n'est nécessaire
	}
	else
		// Le chargement a échoué
		std::cout << "Failed to load skybox" << std::endl;

	// Ensuite, nous allons rajouter un modéle é notre scéne.

	// Les modéles représentent, globalement, tout ce qui est visible en trois dimensions.
	// Nous choisirons ici un vaisseau spatial (Quoi de mieux pour une scéne spatiale ?)

	// Encore une fois, nous récupérons une référence plutét que l'objet lui-méme (cela va étre trés utile par la suite)
	Nz::ModelRef spaceshipModel = Nz::Model::New();

	// Nous allons charger notre modéle depuis un fichier, mais nous pouvons ajuster le modéle lors du chargement via
	// une structure permettant de paramétrer le chargement des modéles
	Nz::ModelParameters params;

	// Le format OBJ ne précise aucune échelle pour ses données, contrairement é Nazara (une unité = un métre en 3D).
	// Comme le vaisseau est trés grand (Des centaines de métres de long), nous allons le rendre plus petit pour les besoins de la démo.
	// Ce paramétre sert é indiquer la mise é l'échelle désirée lors du chargement du modéle.
	params.mesh.matrix.MakeScale(Nz::Vector3f(0.01f));
	// Un centiéme de la taille originelle

	// Les UVs de ce fichier sont retournées (repére OpenGL, origine coin bas-gauche) par rapport é ce que le moteur attend (haut-gauche)  
	// Nous devons donc indiquer au moteur de les retourner lors du chargement
	params.mesh.texCoordScale.Set(1.f, -1.f);

	// Nazara va par défaut optimiser les modéles pour un rendu plus rapide, cela peut prendre du temps et n'est pas nécessaire ici
	params.mesh.optimizeIndexBuffers = false;

	// On charge ensuite le modéle depuis son fichier
	// Le moteur va charger le fichier et essayer de retrouver les fichiers associés (comme les matériaux, textures, ...)
	if (!spaceshipModel->LoadFromFile("resources/Spaceship/spaceship.obj", params))
	{
		// Si le chargement a échoué (fichier inexistant/invalide), il ne sert é rien de continuer
		std::cout << "Failed to load spaceship" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// Nous voulons afficher quelques statistiques relatives au modéle, comme le nombre de sommets et de triangles
	// Pour cela, nous devons accéder au mesh (maillage 3D)

	// Note: Si nous voulions stocker le mesh pour nous en servir aprés, nous devrions alors récupérer une référence pour nous assurer
	// qu'il ne sera pas supprimé tant que nous l'utilisons, mais ici nous faisons un accés direct et ne nous servons plus du pointeur par la suite
	// Il est donc acceptable d'utiliser un pointeur nu ici.
	Nz::Mesh* mesh = spaceshipModel->GetMesh();
	std::cout << mesh->GetVertexCount() << " sommets" << std::endl;
	std::cout << mesh->GetTriangleCount() << " triangles" << std::endl;

	// En revanche, le format OBJ ne précise pas l'utilisation d'une normal map, nous devons donc la charger manuellement
	// Pour commencer on récupére le matériau du mesh, celui-ci en posséde plusieurs mais celui qui nous intéresse,
	// celui de la coque, est le second (Cela est bien entendu lié au modéle en lui-méme)
	Nz::Material* material = spaceshipModel->GetMaterial(1); // Encore une fois nous ne faisons qu'un accés direct.

	// On lui indique ensuite le chemin vers la normal map
	if (!material->SetNormalMap("resources/Spaceship/Texture/normal.png"))
	{
		// Le chargement a échoué, peut-étre le fichier n'existe pas, ou n'est pas reconnu par le moteur
		// Mais ce n'est pas une erreur critique, le rendu peut quand méme se faire (Mais sera moins détaillé)
		std::cout << "Failed to load normal map" << std::endl;
	}

	// Bien, nous avons un modéle valide, mais celui-ci ne consiste qu'en des informations de rendu, de matériaux et de textures.
	// Commenéons donc par créer une entité vide, cela se fait en demandant au monde de générer une nouvelle entité.
	Ndk::EntityHandle spaceship = world->CreateEntity();

	// Note: Nous ne récupérons pas l'entité directement mais un "handle" vers elle, ce dernier est un pointeur intelligent non-propriétaire.
	// Pour des raisons techniques, le pointeur de l'entité peut venir é changer, ou l'entité étre simplement détruite pour n'importe quelle raison.
	// Le Handle nous permet de maintenir un pointeur valide vers notre entité, et invalidé automatiquement é sa mort.

	// Nous avons désormais une entité, mais celle-ci ne contient rien et n'a d'autre propriété qu'un identifiant
	// Nous devons donc lui rajouter les composants que nous voulons.

	// Un NodeComponent donne é notre entité une position, rotation, échelle, et nous permet de l'attacher é d'autres entités (ce que nous ne ferons pas ici).
	// étant donné que par défaut, un NodeComponent se place en (0,0,0) sans rotation et avec une échelle de 1,1,1 et que cela nous convient,
	// nous n'avons pas besoin d'agir sur le composant créé.
	spaceship->AddComponent<Ndk::NodeComponent>();

	// Bien, notre entité nouvellement créé dispose maintenant d'une position dans la scéne, mais est toujours invisible
	// Nous lui ajoutons donc un GraphicsComponent
	Ndk::GraphicsComponent& spaceshipGraphics = spaceship->AddComponent<Ndk::GraphicsComponent>();

	// Ce composant sert de point d'attache pour tous les renderables instanciés (tels que les modéles, les sprites, le texte, etc.)
	// Cela signifie également qu'un modéle peut étre attaché é autant d'entités que nécessaire.
	// Note: Afin de maximiser les performances, essayez d'avoir le moins de renderables instanciés/matériaux et autres ressources possible
	// le moteur fonctionne selon le batching et regroupera par exemple tous les modéles identiques ensembles lors du rendu.
	spaceshipGraphics.Attach(spaceshipModel);

	// Nous avons besoin également d'une caméra pour servir de point de vue é notre scéne, celle-ci sera é l'écart du modéle
	// regardant dans sa direction.

	// On conserve la rotation é part via des angles d'eulers pour la caméra free-fly
	Nz::EulerAnglesf camAngles(0.f, -20.f, 0.f);

	// Nous créons donc une seconde entité
	// Note: La création d'entité est une opération légére au sein du moteur, mais plus vous aurez d'entités et plus le processeur devra travailler.
	Ndk::EntityHandle camera = world->CreateEntity();

	// Notre caméra a elle aussi besoin d'étre positionnée dans la scéne
	Ndk::NodeComponent& cameraNode = camera->AddComponent<Ndk::NodeComponent>();
	cameraNode.SetPosition(0.f, 0.25f, 2.f); // On place la caméra é l'écart
	cameraNode.SetRotation(camAngles);

	// Et dispose d'un composant pour chaque point de vue de la scéne, le CameraComponent
	Ndk::CameraComponent& cameraComp = camera->AddComponent<Ndk::CameraComponent>();

	// Ajoutons un composant écouteur, si nous venions é avoir du son
	camera->AddComponent<Ndk::ListenerComponent>();

	// Et on n'oublie pas de définir les plans délimitant le champs de vision
	// (Seul ce qui se trouvera entre les deux plans sera rendu)

	// La distance entre l'oeil et le plan éloigné
	cameraComp.SetZFar(5000.f);

	// La distance entre l'oeil et le plan rapproché (0 est une valeur interdite car la division par zéro l'est également)
	cameraComp.SetZNear(0.1f);

	// Attention que le ratio entre les deux (zFar/zNear) doit rester raisonnable, dans le cas contraire vous risquez un phénoméne
	// de "Z-Fighting" (Impossibilité de déduire quelle surface devrait apparaétre en premier) sur les surfaces éloignées.

	// Il ne nous manque plus maintenant que de l'éclairage, sans quoi la scéne sera complétement noire
	// Il existe trois types de lumiéres:
	// -DirectionalLight: Lumiére infinie sans position, envoyant de la lumiére dans une direction particuliére
	// -PointLight: Lumiére située é un endroit précis, envoyant de la lumiére finie dans toutes les directions
	// -SpotLight: Lumiére située é un endroit précis, envoyant de la lumiére vers un endroit donné, avec un angle de diffusion

	// Nous allons créer une lumiére directionnelle pour représenter la nébuleuse de notre skybox
	// Encore une fois, nous créons notre entité
	Ndk::EntityHandle nebulaLight = world->CreateEntity();

	// Lui ajoutons une position dans la scéne
	Ndk::NodeComponent& nebulaLightNode = nebulaLight->AddComponent<Ndk::NodeComponent>();

	// Et ensuite le composant principal, le LightComponent
	Ndk::LightComponent& nebulaLightComp = nebulaLight->AddComponent<Ndk::LightComponent>(Nz::LightType_Directional);

	// Il nous faut ensuite configurer la lumiére
	// Pour commencer, sa couleur, la nébuleuse étant d'une couleur jaune, j'ai choisi ces valeurs
	nebulaLightComp.SetColor(Nz::Color(255, 182, 90));

	// Nous appliquons ensuite une rotation de sorte que la lumiére dans la méme direction que la nébuleuse
	nebulaLightNode.SetRotation(Nz::EulerAnglesf(0.f, 102.f, 0.f));

	// Nous allons maintenant créer la fenétre, dans laquelle nous ferons nos rendus
	// Celle-ci demande des paramétres plus complexes

	// Pour commencer le mode vidéo, celui-ci va définir la taille de la zone de rendu et le nombre de bits par pixels
	Nz::VideoMode mode = Nz::VideoMode::GetDesktopMode();
	// Nous récupérons le mode vidéo du bureau

	// Nous allons prendre les trois quarts de la résolution du bureau pour notre fenétre
	mode.width = 3 * mode.width / 4;
	mode.height = 3 * mode.height / 4;

	// Maintenant le titre, rien de plus simple...
	Nz::String windowTitle = "Nazara Demo - First scene";

	// Ensuite, le "style" de la fenétre, posséde-t-elle des bordures, peut-on cliquer sur la croix de fermeture,
	// peut-on la redimensionner, ...
	Nz::WindowStyleFlags style = Nz::WindowStyle_Default; // Nous prenons le style par défaut, autorisant tout ce que je viens de citer

	// Ensuite, les paramétres du contexte de rendu
	// On peut configurer le niveau d'antialiasing, le nombre de bits du depth buffer et le nombre de bits du stencil buffer						  
	// Nous désirons avoir un peu d'antialiasing (4x), les valeurs par défaut pour le reste nous conviendrons trés bien
	Nz::RenderTargetParameters parameters;
	parameters.antialiasingLevel = 4;

	Nz::RenderWindow& window = application.AddWindow<Nz::RenderWindow>(mode, windowTitle, style, parameters);
	if (!window.IsValid())
	{
		std::cout << "Failed to create render window" << std::endl;
		std::getchar();

		return EXIT_FAILURE;
	}

	// On fait disparaétre le curseur de la souris
	window.SetCursor(Nz::SystemCursor_None);

	// On lie la caméra é la fenétre
	cameraComp.SetTarget(&window);

	// Et on créé une horloge pour gérer le temps
	Nz::Clock updateClock;
	Nz::UInt64 updateAccumulator = 0;

	// Quelques variables de plus pour notre caméra
	bool smoothMovement = true;
	Nz::Vector3f targetPos = cameraNode.GetPosition();

	application.EnableConsole(true);
	application.EnableFPSCounter(true);

	Ndk::Application::ConsoleOverlay& consoleOverlay = application.GetConsoleOverlay();
	consoleOverlay.lua.PushGlobal("Spaceship", spaceship->CreateHandle());
	consoleOverlay.lua.PushGlobal("World", world->CreateHandle());

	//Gestion des Evenements 
	Nz::EventHandler& eventHandler = window.GetEventHandler();

	eventHandler.OnMouseMoved.Connect([&camAngles, &cameraNode, &window](const Nz::EventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
	{
		if (Ndk::Application::Instance()->IsConsoleEnabled())
		{
			Ndk::Application::ConsoleOverlay& consoleOverlay = Ndk::Application::Instance()->GetConsoleOverlay();
			if (consoleOverlay.console->IsVisible())
				return;
		}
		// Gestion de la caméra free-fly (Rotation)
		float sensitivity = 0.3f; // Sensibilité de la souris

		// On modifie l'angle de la caméra gréce au déplacement relatif sur X de la souris
		camAngles.yaw = Nz::NormalizeAngle(camAngles.yaw - event.deltaX*sensitivity);

		// Idem, mais pour éviter les problémes de calcul de la matrice de vue, on restreint les angles
		camAngles.pitch = Nz::Clamp(camAngles.pitch - event.deltaY*sensitivity, -89.f, 89.f);

		// On applique les angles d'Euler é notre caméra
		cameraNode.SetRotation(camAngles);

		// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenétre
		// Cette fonction est codée de sorte é ne pas provoquer d'événement MouseMoved
		Nz::Vector2ui size = window.GetSize();
		Nz::Mouse::SetPosition(size.x / 2, size.y / 2, window);
	});

	eventHandler.OnKeyPressed.Connect([&targetPos, &cameraNode, &smoothMovement, &window](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& event)
	{
		// Une touche a été pressée !
		if (event.code == Nz::Keyboard::Key::Escape)
			window.Close();
		else if (event.code == Nz::Keyboard::F1)
		{
			if (smoothMovement)
			{
				targetPos = cameraNode.GetPosition();
				smoothMovement = false;
			}
			else
				smoothMovement = true;
		}
	});

	// Début de la boucle de rendu du programme (s'occupant par exemple de mettre é jour le monde)
	while (application.Run())
	{

		Nz::UInt64 elapsedUS = updateClock.GetMicroseconds();
		// On relance l'horloge
		updateClock.Restart();

		// Mise é jour (Caméra)
		const Nz::UInt64 updateRate = 1000000 / 60; // 60 fois par seconde
		updateAccumulator += elapsedUS;

		if (updateAccumulator >= updateRate)
		{
			// Le temps écoulé en seconde depuis la derniére fois que ce bloc a été exécuté
			float elapsedTime = updateAccumulator / 1000000.f;

			// Vitesse de déplacement de la caméra
			float cameraSpeed = 3.f * elapsedTime; // Trois métres par seconde

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

				// Pour que nos déplacement soient liés é la rotation de la caméra, nous allons utiliser
				// les directions locales de la caméra

				// Si la fléche du haut ou la touche Z (vive ZQSD !!) est pressée, on avance
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Z))
					targetPos += cameraNode.GetForward() * cameraSpeed;

				// Si la fléche du bas ou la touche S est pressée, on recule
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

				// Contréle (Gauche ou droite) pour descendre dans l'espace global, etc...
				if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::RControl))
					targetPos += Nz::Vector3f::Down() * cameraSpeed;
			}

			cameraNode.SetPosition((smoothMovement) ? DampedString(cameraNode.GetPosition(), targetPos, elapsedTime) : targetPos, Nz::CoordSys_Global);
			updateAccumulator = 0;
		}

		// Aprés avoir dessiné sur la fenétre, il faut s'assurer qu'elle affiche cela
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
	// Tout le mérite revient é l'auteur (Qui me permettra ainsi d'améliorer les démos, voire méme le moteur)

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
	float springMagitude = springStrength * displacementLength + dampConstant * invDisplacementLength;

	// Normalise the displacement and scale by the spring magnitude
	// and the amount of time passed
	float scalar = std::min(invDisplacementLength * springMagitude * frametime, 1.f);
	displacement *= scalar;

	// move the camera a bit towards the target
	return currentPos + displacement;
}
