#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <iostream>
#include <map>

// Une structure pour contenir nos informations (Cette structure est très simpliste)
struct Model
{
	NzMatrix4f matrix; // Les transformations subies par le modèle
	NzMesh mesh; // Le mesh
	NzTexture texture; // Sa texture
};

struct AnimatedModel : public Model
{
	// Quelques variables pour l'animation
	const NzSequence* currentSequence = nullptr; // La séquence en cours
	float interpolation = 0.f; // La valeur de l'interpolation ([0..1], si dépasse 1, on passe à la frame suivante)
	unsigned int currentFrame = 0; // La première frame
	unsigned int nextFrame; // La seconde frame, l'animation est interpollée entre ces deux-là
};

void AnimateModel(AnimatedModel& moedel, float elapsed);
bool CreateCheckerTexture(NzTexture* texture);
bool CreateFloorMesh(NzMesh* mesh);
void DrawModel(const Model& model);
void SetSequence(AnimatedModel& model, const NzString& name);

int main()
{
	// Tout d'abord on affiche les instructions
	std::cout << "Controls: ZQSD" << std::endl;
	std::cout << "Escape to quit" << std::endl;
	std::cout << "Left click to capture/free the mouse" << std::endl;
	std::cout << "Right click to control Dr. Freak" << std::endl;

	// Cette ligne active le mode de compatibilité d'OpenGL lors de l'initialisation de Nazara (Nécessaire pour le shader)
	NzContextParameters::defaultCompatibilityProfile = true;

	// Maintenant nous initialisons le Renderer (Qui initialisera le noyau ainsi que le module utilitaire)
	// Cette étape est obligatoire pour beaucoup de fonctionnalités (Notamment le chargement de ressources et le rendu)
	NzInitializer<NzRenderer> renderer;
	if (!renderer)
	{
		// Ça n'a pas fonctionné, le pourquoi se trouve dans le fichier NazaraLog.log
		std::cout << "Failed to initialize Nazara, see NazaraLog.log for further informations" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	// Maintenant nous pouvons utiliser le moteur comme bon nous semble, tout d'abord nous allons charger les ressources

	// Charger une ressource se fait actuellement manuellement, mais un ResourceManager est à venir
	// Vous initialisez une ressource, et la chargez via sa méthode LoadFrom[File|Memory|Stream]
	// Note: il est possible de donner des instructions au loader (qui va charger le fichier en ressource) via les ResourceParameters
	NzMeshParams parameters;

	// Le loader doit-il automatiquement charger les animations ?
	// Attention, ce paramètre possède une signification différente selon le type d'animation du mesh.
	// -Pour les animations keyframe (image-clé), c'est la seule et unique façon de charger les animations, étant donné
	// qu'elles sont fourniees avec le mesh.
	// -Pour les animations squelettiques, le loader ne fera que charger automatiquement l'animation associée au mesh s'il le peut
	// Dans les deux cas, les paramètres d'animations (parameters.animation) seront utilisés
	parameters.loadAnimations = true; // Vaut true par défaut

	// Pour qu'un mesh puisse être rendu, il doit être stocké du côté de la carte graphique (Hardware), mais il est parfois utile de
	// le stocker côté RAM, par exemple pour le moteur physique. En sachant qu'il est facile de changer le stockage d'un buffer.
	parameters.storage = nzBufferStorage_Hardware; // Vaut nzBufferStorage_Hardware par défaut

	AnimatedModel drfreak;
	if (!drfreak.mesh.LoadFromFile("resources/drfreak.md2", parameters)) // On charge notre bon vieux docteur avec les paramètres de chargement.
	{
		// Le chargement n'a pas fonctionné, le modèle est peut-être corrompu/non-supporté, ou alors n'existe pas.
		std::cout << "Failed to load mesh" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	if (!drfreak.mesh.HasAnimation()) // Le mesh possède-t-il des animations ?
	{
		// Cette démo n'a aucun intérêt sans animations
		std::cout << "Mesh has no animation" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	SetSequence(drfreak, "stand");

	// Il est possible que le mesh possède un ou plusieurs skin, nous utiliserons cette information pour charger une texture
	if (drfreak.mesh.HasSkin())
	{
		// Contrairement aux autres ressources, la texture n'est pas critique
		if (drfreak.texture.LoadFromFile("resources/" + drfreak.mesh.GetSkin()))
			drfreak.texture.SetFilterMode(nzTextureFilter_Bilinear); // Appliquons-lui un filtrage bilinéaire
		else
			std::cout << "Failed to load texture" << std::endl;
	}

	if (!drfreak.texture.IsValid()) // Les méthodes Resource::IsValid indiquent si la ressource a été correctement créée
	{
		std::cout << "Creating checker texture for mesh" << std::endl;

		if (!CreateCheckerTexture(&drfreak.texture))
		{
			std::cout << "Failed to create mesh texture" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}
	}

	// Nous créons maintenant notre sol
	Model floor;
	if (!CreateFloorMesh(&floor.mesh))
	{
		std::cout << "Failed to create floor" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	if (!CreateCheckerTexture(&floor.texture))
	{
		std::cout << "Failed to create floor texture" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Le sol ne subit aucune transformation
	floor.matrix.MakeIdentity();

	// Pour effectuer un rendu, il faut que la carte graphique sache comment le faire.
	// Les shaders sont de petits programmes qui donnent des instructions à la carte graphique lors de son pipeline.
	// Ils sont aujourd'hui indispensables pour un rendu 3D, mais sont très utiles pour divers effets !
	// Il existe plusieurs langages de shaders, GLSL pour OpenGL, HLSL pour Direct3D et Cg qui peut être utilisé pour les deux.
	// Le Renderer de Nazara utilise OpenGL, par conséquent nous utiliserons le GLSL
	// La méthode NzShader::IsLanguageSupported permet de savoir si un langage est supporté.
	NzShader shader;
	if (!shader.Create(nzShaderLanguage_GLSL))
	{
		std::cout << "Failed to load shader" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Une fois le shader créé, nous devons lui spécifier les codes sources de nos shaders
	// Pour notre exemple nous prendrons un shader très simple
	// Un shader doit obligatoirement posséder au moins deux codes, un pour le fragment shader et un pour le vertex shader

	// Le fragment shader traite la couleur de nos pixels
	if (!shader.LoadFromFile(nzShaderType_Fragment, "shaders/basic.frag"))
	{
		std::cout << "Failed to load fragment shader from file" << std::endl;
		// À la différence des autres ressources, le shader possède un log qui peut indiquer les erreurs en cas d'échec
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Le vertex shader (Transformation des vertices de l'espace 3D vers l'espace écran)
	if (!shader.LoadFromFile(nzShaderType_Vertex, "shaders/basic.vert"))
	{
		std::cout << "Failed to load vertex shader from file" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Une fois les codes sources de notre shader chargé, nous pouvons le compiler, afin de le rendre utilisable
	if (!shader.Compile())
	{
		std::cout << "Failed to compile shader" << std::endl;
		std::cout << "Log: " << shader.GetLog() << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Nos ressources sont chargées, et c'est bien beau, mais il nous faudrait une fenêtre pour afficher tout ça
	// Window représente une fenêtre singulière, pour y effectuer un rendu il nous faut une RenderWindow
	// Tout d'abord, sa taille, disons celle du bureau divisé par deux

	// Un VideoMode est une structure contenant une longueur (width), une largeur (height) et le nombre de bits par pixels (bitsPerPixel)
	NzVideoMode mode = NzVideoMode::GetDesktopMode(); // Nous récupérons le mode actuellement utilisé par le bureau

	// Nous divisons sa longueur et sa largeur par deux
	mode.width /= 2;
	mode.height /= 2;

	// Maintenant le titre, rien de plus simple...
	NzString windowTitle = "Nazara Demo - AnimatedMesh";

	// Nous pouvons créer notre fenêtre ! (Via le constructeur directement ou par la méthode Create)
	NzRenderWindow window;

	// Le premier argument définit la taille de rendu de la fenêtre (Si elle possède une bordure elle sera légèrement plus grande).
	// Le deuxième argument est le titre de la fenêtre lors de sa création, vous pouvez le modifier à tout moment via window.SetTitle.
	// Le troisième argument représente la décoration de la fenêtre, sa bordure, ses boutons.
	// Attention que cela permet à la fenêtre de changer sa taille et qu'il faudra donc traiter l'évènement.
	// Par défaut le troisième argument vaut nzWindowStyle_Default (Bordure + Bouton de fermeture + Redimensionnement)
	if (!window.Create(mode, windowTitle, nzWindowStyle_Default))
	{
		std::cout << "Failed to create window" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Notre belle fenêtre est créée, nous pouvons la configurer

	// On cache le curseur
	window.SetCursor(nzWindowCursor_None);

	// Nous limitons les FPS à 100
	window.SetFramerateLimit(100);

	// La matrice de projection définit la transformation du vertice 3D à un point 2D
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 1000.f));

	// Notre fenêtre est créée, cependant il faut s'occuper d'elle, pour le rendu et les évènements
	NzClock secondClock, updateClock; // Des horloges pour gérer le temps
	unsigned int fps = 0; // Compteur de FPS

	// Quelques variables pour notre improvisation de physique
	float groundPos = drfreak.mesh.GetAABB().GetMinimum().y; // Les coordonnées locales du "bas" du modèle
	NzVector3f modelPos(0.f, -groundPos, -50.f);
	NzVector3f modelVel(0.f, 0.f, 0.f);
	NzQuaternionf modelOrient(NzQuaternionf::Identity());
	NzEulerAnglesf modelRot(0.f, 0.f, 0.f);
	float modelSpeed = 150.f;

	// Nous initialisons la matrice
	drfreak.matrix = NzMatrix4f::Rotate(modelOrient) * NzMatrix4f::Translate(modelPos);

	// Notre caméra
	NzVector3f camPos(0.f, 25.f, -20.f);
	NzQuaternionf camOrient(NzQuaternionf::Identity());
	NzEulerAnglesf camRot(0.f, 0.f, 0.f); // Les angles d'eulers sont bien plus facile à utiliser
	NzMatrix4f camMatrix = NzMatrix4f::Translate(camPos);
	float camSpeed = 100.f;
	float sensitivity = 0.8f;

	// Quelques variables
	bool camMode = true;
	bool paused = false;
	bool thirdPerson = false;
	bool windowOpen = true;

	// On peut commencer la boucle du programme
	while (windowOpen)
	{
		// Ici nous gérons les évènements
		NzEvent event;
		while (window.PollEvent(&event)) // Avons-nous un évènement dans la file ?
		{
			// Nous avons un évènement !

			switch (event.type) // De quel type est cet évènement ?
			{
				case nzEventType_Quit: // L'utilisateur/L'OS nous a demandé de terminer notre exécution
					windowOpen = false; // Nous terminons alors la boucle
					break;

				case nzEventType_MouseMoved: // La souris a bougé
				{
					// Si nous ne sommes pas en mode free-fly, on ne traite pas l'évènement
					if (!camMode || thirdPerson)
						break;

					// On modifie l'angle de la caméra grâce au déplacement relatif de la souris
					camRot.yaw = NzNormalizeAngle(camRot.yaw - event.mouseMove.deltaX*sensitivity);

					// Pour éviter les loopings mais surtout les problèmes de calculation de la matrice de vue, on restreint les angles
					camRot.pitch = NzClamp(camRot.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					// La matrice vue représente les transformations effectuées par la caméra
					// On recalcule la matrice de la caméra et on l'envoie au renderer
					camOrient = camRot; // Conversion des angles d'euler en quaternion
					NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(camPos, camPos + camOrient * NzVector3f::Forward()));

					// Pour éviter que le curseur ne sorte de l'écran, nous le renvoyons au centre de la fenêtre
					NzMouse::SetPosition(window.GetWidth()/2, window.GetHeight()/2, window);
					break;
				}

				case nzEventType_MouseButtonPressed: // L'utilisateur (ou son chat) vient de cliquer sur la souris
					if (event.mouseButton.button == NzMouse::Left) // Est-ce le clic gauche ?
					{
						// L'utilisateur vient d'appuyer sur le bouton left de la souris
						// Nous allons inverser le mode caméra et montrer/cacher le curseur en conséquence
						if (camMode)
						{
							camMode = false;
							window.SetCursor(nzWindowCursor_Default);
						}
						else
						{
							camMode = true;
							window.SetCursor(nzWindowCursor_None);
						}
					}
					else if (event.mouseButton.button == NzMouse::Right) // Est-ce le clic droit ?
					{
						if (thirdPerson)
						{
							// On arrête le mouvement
							SetSequence(drfreak, "stand");

							// Afin de synchroniser le quaternion avec les angles d'euler
							camRot = camOrient.ToEulerAngles();
							thirdPerson = false;
						}
						else
							thirdPerson = true;
					}
					break;

				case nzEventType_Resized: // L'utilisateur a changé la taille de la fenêtre, le coquin !
					NzRenderer::SetViewport(NzRectui(0, 0, event.size.width, event.size.height)); // Adaptons l'affichage

					// Il nous faut aussi mettre à jour notre matrice de projection
					NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(event.size.width)/event.size.height, 1.f, 1000.f));
					break;

				case nzEventType_KeyPressed: // Une touche du clavier vient d'être enfoncée
					if (thirdPerson &&
						(event.key.code == NzKeyboard::Z || // Est-ce la touche Z ?
						 event.key.code == NzKeyboard::S || // Ou bien la touche S ?
						 event.key.code == NzKeyboard::Q || // Ou encore la touche Q ?
						 event.key.code == NzKeyboard::D))  // Et pourquoi pas la touche D ?
					{
						// Si une touche concernant le déplacement est appuyée
						SetSequence(drfreak, "run"); // On anime le personnage pour qu'il ait une animation de déplacement
					}
					else if (event.key.code == NzKeyboard::Escape)
						windowOpen = false;
					else if (event.key.code == NzKeyboard::P)
						paused = !paused;

					break;

				case nzEventType_KeyReleased: // Une touche du clavier vient d'être relachée
					if (thirdPerson &&
						!NzKeyboard::IsKeyPressed(NzKeyboard::Z) && // Est-ce que la touche Z est enfoncée en ce moment ?
						!NzKeyboard::IsKeyPressed(NzKeyboard::S) && // Ou bien la touche S ?
						!NzKeyboard::IsKeyPressed(NzKeyboard::Q) && // Etc..
						!NzKeyboard::IsKeyPressed(NzKeyboard::D)) // Etc..
					{
						// Si plus aucune touche de déplacement n'est enfoncée
						SetSequence(drfreak, "stand");
					}

					break;

				default: // Les autres évènements, on s'en fiche
					break;
			}
		}

		// Mise à jour de la partie logique
		if (updateClock.GetMilliseconds() >= 1000/60) // 60 fois par seconde
		{
			float elapsedTime = updateClock.GetSeconds(); // Le temps depuis la dernière mise à jour

			// Déplacement de la caméra
			static const NzVector3f forward(NzVector3f::Forward());
			static const NzVector3f left(NzVector3f::Left());
			static const NzVector3f up(NzVector3f::Up());

			// Notre rotation sous forme de quaternion nous permet de tourner un vecteur
			// Par exemple ici, quaternion * forward nous permet de récupérer le vecteur de la direction "avant"

			if (thirdPerson)
			{
				// Nous déplaçons le personnage en fonction des touches pressées

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
					modelPos += modelOrient * forward * modelSpeed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
					modelPos -= modelOrient * forward * modelSpeed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
					modelRot.yaw += camSpeed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
					modelRot.yaw -= camSpeed * elapsedTime;

				modelOrient = modelRot;
			}
			else
			{
				// Sinon, c'est la caméra qui se déplace (en fonction des mêmes touches)

				// Un boost en maintenant le shift gauche
				float speed = (NzKeyboard::IsKeyPressed(NzKeyboard::Key::LShift)) ? camSpeed*5 : camSpeed;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
					camPos += camOrient * forward * speed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
					camPos -= camOrient * forward * speed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
					camPos += camOrient * left * speed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
					camPos -= camOrient * left * speed * elapsedTime;

				// En revanche, ici la hauteur est toujours la même, peu importe notre orientation
				if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
					camPos += up * speed * elapsedTime;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
					camPos -= up * speed * elapsedTime;
			}

			// Oui les quaternions et les matrices sont calculés même si la caméra ne bouge pas
			// C'est une limitation de mon implémentation, qui ne sera pas présente une fois les NzSceneNode intégrés
			if (thirdPerson)
			{
				static NzQuaternionf rotDown(NzEulerAnglesf(-15.f, 0.f, 0.f)); // Une rotation pour regarder vers le bas
				camOrient = modelOrient * rotDown;

				camPos = modelPos + camOrient * NzVector3f(0.f, 25.f, 60.f);
			}

			NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(camPos, camPos + camOrient * NzVector3f::Forward()));

			// Mise à jour de la matrice du personnage
			drfreak.matrix = NzMatrix4f::Rotate(modelOrient) * NzMatrix4f::Translate(modelPos);

			// Animation
			if (!paused)
				AnimateModel(drfreak, elapsedTime);

			updateClock.Restart();
		}

		// On active le shader et paramètrons le rendu
		NzRenderer::SetShader(&shader);

		// Notre scène 3D requiert un test de profondeur
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);

		// Nous voulons avoir un fond bien gris
		NzRenderer::SetClearColor(128, 128, 128);

		// Et nous effaçons les buffers de couleur et de profondeur
		NzRenderer::Clear(nzRendererClear_Color | nzRendererClear_Depth);

		// Affichage des meshs
		DrawModel(floor);

		// Notre Dr. Freak possède des normales, nous pouvons alors éliminer les faces qu'on ne voit pas
		NzRenderer::Enable(nzRendererParameter_FaceCulling, true);

		DrawModel(drfreak);

		NzRenderer::Enable(nzRendererParameter_FaceCulling, false);

		// Nous mettons à jour l'écran
		window.Display();

		fps++;

		// Toutes les secondes
		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " (FPS: " + NzString::Number(fps) + ')');
			fps = 0;
			secondClock.Restart();
		}
	}

    return EXIT_SUCCESS;
}

void AnimateModel(AnimatedModel& model, float elapsed)
{
	model.interpolation += model.currentSequence->framePerSecond * elapsed;
	while (model.interpolation > 1.f)
	{
		model.interpolation -= 1.f;

		model.currentFrame = model.nextFrame;
		if (++model.nextFrame > model.currentSequence->lastFrame)
			model.nextFrame = model.currentSequence->firstFrame;
	}

	model.mesh.Animate(model.currentFrame, model.nextFrame, (NzKeyboard::IsKeyPressed(NzKeyboard::A)) ? 0.f : model.interpolation);
}

bool CreateCheckerTexture(NzTexture* texture)
{
	NzImage image;
	// Nous crééons une image 2D, au format RGBA8 de dimensions 128*128 (8 blocs de 16 pixels de côté)
	if (!image.Create(nzImageType_2D, nzPixelFormat_RGBA8, 8*16, 8*16))
	{
		// Ne devrait pas arriver (La création d'une image ne peut échouer que si l'un des argument est incorrect)
		std::cout << "Failed to create image, this means a bug has been found in Nazara" << std::endl;
		return false;
	}

	// Pour modifier les pixels, nous pouvons accéder directement à ces derniers avec GetPixels(), ou bien à un pixel
	// via [Get|Set]PixelColor, mais pour cette occasion nous utiliserons une méthode bien pratique, Fill.
	unsigned int blockCountX = image.GetWidth()/16;
	unsigned int blockCountY = image.GetHeight()/16;
	for (unsigned int x = 0; x < blockCountX; ++x)
	{
		for (unsigned int y = 0; y < blockCountY; ++y)
		{
			// Une belle texture de damier
			NzColor color = (x%2 == y%2) ? NzColor::White : NzColor::Black;
			// Fill remplit une zone de la texture avec une couleur
			image.Fill(color, NzRectui(x*16, y*16, 16, 16));
		}
	}

	if (!texture->LoadFromImage(image)) // Nous créons notre texture depuis notre image
	{
		// Nous n'avons vraiment pas beaucoup de chance..
		std::cout << "Failed to load image" << std::endl;
		return false;
	}

	texture->SetAnisotropyLevel(NzRenderer::GetMaxAnisotropyLevel()); // Un filtrage anisotropique pour la texture
	texture->SetWrapMode(nzTextureWrap_Repeat); // Si les coordonnées de texture dépassent 1.f, la texture sera répétée

	return true;
}

bool CreateFloorMesh(NzMesh* mesh)
{
	// Cette fonction créé un mesh statique simpliste pour servir de sol

	// Nous créons un mesh statique
	if (!mesh->Create(nzAnimationType_Static))
	{
		// L'échec est techniquement impossible mais le moteur étant en constante évolution ...
		std::cout << "Failed to create mesh" << std::endl;
		return false;
	}

	// Les vertex declaration ont pour seul but de décrire l'agencement d'un vertex buffer
	// Elles sont composées de VertexElement, chacun décrivant un élément du buffer
	NzVertexDeclaration* declaration = new NzVertexDeclaration;

	// Il y a cinq paramètres différents (stream, usage, type, offset, usageIndex)
	// -Stream: À quoi serviront les données ? À définir des sommets (nzElementStream_VertexData) ou à l'instancing (nzElementStream_InstancedData)
	// -Usage: Comment cette donnée doit-elle être envoyée au shader
	// -Type: Comment sont stockées ces données ? (Un triplet de float ? Deux double ? ..)
	// -Offset: La position de la donnée dans le buffer (les données sont entrelacées)
	// -UsageIndex: Pour les coordonnées de texture, définit l'indice de texture utilisé.
	NzVertexElement elements[2];
	elements[0].usage = nzElementUsage_Position; // Notre premier élément sera la position des vertices
	elements[0].offset = 0; // Celles-ci sont placées au début
	elements[0].type = nzElementType_Float3; // Sont composées de trois flottants

	elements[1].usage = nzElementUsage_TexCoord;
	elements[1].offset = 3*sizeof(float);
	elements[1].type = nzElementType_Float2;

	if (!declaration->Create(elements, 2))
	{
		// Nos éléments sont invalides !
		std::cout << "Failed to create vertex declaration" << std::endl;
		return false;
	}

	// Nous créons ensuite un buffer de 4 vertices (le second argument précise l'espace pris par chaque vertex), le stockage
	// Et nous indiquons que nous n'y toucherons plus
	NzVertexBuffer* buffer = new NzVertexBuffer(4, declaration->GetStride(nzElementStream_VertexData), nzBufferStorage_Hardware, nzBufferUsage_Static);

	// Doit respecter la declaration
	float vertices[] =
	{
		// Vertex 1
		-1000.f, 0.f, -1000.f, // Position
		0.f, 0.f,          // UV

		// Vertex 2
		-1000.f, 0.f, 1000.f, // Position
		0.f, 10.f,          // UV

		// Vertex 3
		1000.f, 0.f, -1000.f, // Position
		10.f, 0.f,          // UV

		// Vertex 4
		1000.f, 0.f, 1000.f, // Position
		10.f, 10.f           // UV
	};

	// Afin de modifier un buffer, il nous faut soit le verrouiller (accès bas-niveau),  soit le remplir (accès de plus haut niveau)
	if (!buffer->Fill(vertices, 0, 4)) // Nous remplissons à partir de l'index 0, et nous envoyons 4 vertices
	{
		std::cout << "Failed to fill buffer" << std::endl;
		return false;
	}

	NzStaticMesh* subMesh = new NzStaticMesh(mesh);
	if (!subMesh->Create(declaration, buffer))
	{
		std::cout << "Failed to create subMesh" << std::endl;
		return false;
	}

	subMesh->SetPrimitiveType(nzPrimitiveType_TriangleStrip);

	// On ajoute le submesh au mesh
	mesh->AddSubMesh(subMesh);

	// Nos ressources sont notifiées utilisées par le mesh et le submesh, nous pouvons les rendre éphèmères.
	// Les ressources seront donc automatiquement libérées lorsqu'elles ne seront plus référencées par une classe
	buffer->SetPersistent(false);
	declaration->SetPersistent(false);
	subMesh->SetPersistent(false); // Pour le submesh, c'est déjà le comportement par défaut

	return true;
}

void DrawModel(const Model& model)
{
	// La matrice world est celle qui représente les transformations du modèle
	NzRenderer::SetMatrix(nzMatrixType_World, model.matrix);

	NzShader* shader = NzRenderer::GetShader();// On récupère le shader du rendu
	shader->SendTexture(shader->GetUniformLocation("texture"), &model.texture);

	// Un mesh est divisé en plusieurs submeshes
	unsigned int subMeshCount = model.mesh.GetSubMeshCount();
	for (unsigned int i = 0; i < subMeshCount; ++i)
	{
		// On récupère le submesh
		const NzSubMesh* subMesh = model.mesh.GetSubMesh(i);

		// On paramètre le Renderer avec ses données
		NzRenderer::SetIndexBuffer(subMesh->GetIndexBuffer());
		NzRenderer::SetVertexBuffer(subMesh->GetVertexBuffer());
		NzRenderer::SetVertexDeclaration(subMesh->GetVertexDeclaration());

		// On fait le rendu
		NzRenderer::DrawPrimitives(subMesh->GetPrimitiveType(), 0, subMesh->GetVertexCount());
	}
}

void SetSequence(AnimatedModel& model, const NzString& sequenceName)
{
	// On récupère l'animation du mesh
	const NzAnimation* animation = model.mesh.GetAnimation();

	// Nous nous basons sur l'assertion que la séquence existe (Chose que nous pouvons tester avec HasSequence())
	const NzSequence* sequence = animation->GetSequence(sequenceName);
	if (model.currentSequence != sequence)
	{
		model.currentSequence = sequence;

		// Pour avoir une interpolation entre la séquence précédente et celle-ci, nous n'affectons que nextFrame
		model.nextFrame = model.currentSequence->firstFrame;
	}
}
