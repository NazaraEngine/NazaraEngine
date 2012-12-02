#include <Nazara/3D/Model.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <iostream>
#include <map>

bool CreateCheckerMaterial(NzMaterial* material);
bool CreateFloorModel(NzModel* model);
void DrawModel(const NzModel& model);

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

	NzDebugDrawer::Initialize();

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
	parameters.animated = true; // Vaut true par défaut

	// Pour qu'un mesh puisse être rendu, il doit être stocké du côté de la carte graphique (Hardware), mais il est parfois utile de
	// le stocker côté RAM, par exemple pour le moteur physique. En sachant qu'il est facile de changer le stockage d'un buffer.
	parameters.storage = nzBufferStorage_Hardware; // Vaut nzBufferStorage_Hardware par défaut si possible et nzBufferStorage_Software autrement.

	NzModel drfreak;
	if (!drfreak.LoadFromFile("resources/drfreak.md2")) // On charge notre bon vieux docteur avec les paramètres de chargement.
	{
		// Le chargement n'a pas fonctionné, le modèle est peut-être corrompu/non-supporté, ou alors n'existe pas.
		std::cout << "Failed to load Dr. Freak" << std::endl;
		std::getchar(); // On laise le temps de voir l'erreur
		return EXIT_FAILURE;
	}

	if (!drfreak.HasAnimation()) // Le mesh possède-t-il des animations ?
	{
		// Cette démo n'a aucun intérêt sans animations
		std::cout << "Mesh has no animation" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	// Nous créons maintenant notre sol
	NzModel floor;
	if (!CreateFloorModel(&floor))
	{
		std::cout << "Failed to create floor" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

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

	// Nous allons prendre les trois quarts de la résolution du bureau pour notre fenêtre
	//mode.width *= 3.f/4.f;
	//mode.height *= 3.f/4.f;
	mode.width = 1280;
	mode.height = 720;
	// 720p power !

	// Maintenant le titre, rien de plus simple...
	NzString windowTitle = "Nazara Demo - Skeletal mesh test";

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
	//window.SetFramerateLimit(100);

	// La matrice de projection définit la transformation du vertice 3D à un point 2D
	NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(window.GetWidth())/window.GetHeight(), 1.f, 10000.f));

	// Notre fenêtre est créée, cependant il faut s'occuper d'elle, pour le rendu et les évènements
	unsigned int fps = 0; // Compteur de FPS

	// Quelques variables pour notre improvisation de physique
	NzEulerAnglesf modelRot(0.f, 0.f, 0.f);
	float modelSpeed = 250.f;

	// Notre caméra
	NzEulerAnglesf camRot(0.f, 180.f, 0.f); // Les angles d'eulers sont bien plus facile à utiliser

	NzNode camera;
	camera.SetTranslation(0.f, 50.f, -50.f);
	camera.SetRotation(camRot);

	NzVector3f camSpeed(100.f);
	float sensitivity = 0.8f;

	// Quelques variables
	bool camMode = true;
	bool paused = false;
	bool thirdPerson = false;
	bool windowOpen = true;

	NzClock loadClock;

	NzModel hellknight;
	//if (!LoadModel("resources/mm/snoutx10k.md5mesh", params, &hellknight))
	//if (!LoadModel("resources/Boblamp/boblampclean.md5mesh", params, &hellknight))
	if (!hellknight.LoadFromFile("resources/hellknight.md5mesh"))
	{
		std::cout << "Failed to load mesh" << std::endl;
		return 0;
	}

	NzAnimation* hellknightAnimation = new NzAnimation;
	//if (!hellknightAnimation.LoadFromFile("resources/mm/idle.md5anim"))
	//if (!hellknightAnimation.LoadFromFile("resources/Boblamp/boblampclean.md5anim"))0
	if (!hellknightAnimation->LoadFromFile("resources/hellknight/walk7.md5anim"))
	{
		std::cout << "Failed to load animation" << std::endl;
		delete hellknightAnimation;

		return 0;
	}

	hellknightAnimation->SetPersistent(false, false);
	hellknight.SetAnimation(hellknightAnimation);

	std::cout << "Loaded in " << loadClock.GetSeconds() << std::endl;

	bool drawAabb = false;
	bool drawSkeleton = false;
	bool drawHellknight = true;
	bool drawWireframe = false;

	NzClock secondClock, updateClock; // Des horloges pour gérer le temps

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
					camera.SetRotation(camRot); // Conversion des angles d'euler en quaternion

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
							drfreak.SetSequence("stand");

							// Afin de synchroniser le quaternion avec les angles d'euler
							camRot = camera.GetDerivedRotation().ToEulerAngles();

							thirdPerson = false;
						}
						else
							thirdPerson = true;
					}
					break;

				case nzEventType_Resized: // L'utilisateur a changé la taille de la fenêtre, le coquin !
					NzRenderer::SetViewport(NzRectui(0, 0, event.size.width, event.size.height)); // Adaptons l'affichage

					// Il nous faut aussi mettre à jour notre matrice de projection
					NzRenderer::SetMatrix(nzMatrixType_Projection, NzMatrix4f::Perspective(NzDegrees(70.f), static_cast<float>(event.size.width)/event.size.height, 1.f, 10000.f));
					break;

				case nzEventType_KeyPressed: // Une touche du clavier vient d'être enfoncée
				{
					switch (event.key.code)
					{
						case NzKeyboard::Z:
						case NzKeyboard::S:
						case NzKeyboard::Q:
						case NzKeyboard::D:
							if (thirdPerson)
								drfreak.SetSequence("run");
							break;

						case NzKeyboard::Escape:
							windowOpen = false;
							break;

						case NzKeyboard::P:
							paused = !paused;
							break;

						case NzKeyboard::F1:
							if (drawWireframe)
							{
								drawWireframe = false;
								NzRenderer::SetFaceFilling(nzFaceFilling_Fill);
							}
							else
							{
								drawWireframe = true;
								NzRenderer::SetFaceFilling(nzFaceFilling_Line);
							}
							break;

						case NzKeyboard::F2:
							drawAabb = !drawAabb;
							break;

						case NzKeyboard::F3:
							drawSkeleton = !drawSkeleton;
							break;

						case NzKeyboard::F4:
							drawHellknight = !drawHellknight;
							break;

						/*case NzKeyboard::F5:
						{
							NzString animationName;
							std::cin >> animationName;
							if (!hellknightAnimation.LoadFromFile("resources/mm/" + animationName + ".md5anim"))
							{
								std::cout << "Failed to load animation" << std::endl;
								break;
							}

							SetSequence(hellknight, 0);

							break;
						}*/

						default:
							break;
					}

					break;
				}

				case nzEventType_KeyReleased: // Une touche du clavier vient d'être relachée
					if (thirdPerson &&
						!NzKeyboard::IsKeyPressed(NzKeyboard::Z) && // Est-ce que la touche Z est enfoncée en ce moment ?
						!NzKeyboard::IsKeyPressed(NzKeyboard::S) && // Ou bien la touche S ?
						!NzKeyboard::IsKeyPressed(NzKeyboard::Q) && // Etc..
						!NzKeyboard::IsKeyPressed(NzKeyboard::D)) // Etc..
					{
						// Si plus aucune touche de déplacement n'est enfoncée
						drfreak.SetSequence("stand");
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
					drfreak.Translate(forward * modelSpeed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
					drfreak.Translate(-forward * modelSpeed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
					drfreak.Rotate(NzEulerAnglesf(0.f, modelSpeed * elapsedTime, 0.f));

				if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
					drfreak.Rotate(NzEulerAnglesf(0.f, -modelSpeed * elapsedTime, 0.f));
			}
			else
			{
				// Sinon, c'est la caméra qui se déplace (en fonction des mêmes touches)

				// Un boost en maintenant le shift gauche
				NzVector3f speed = (NzKeyboard::IsKeyPressed(NzKeyboard::Key::LShift)) ? camSpeed*5 : camSpeed;

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Z))
					camera.Translate(forward * speed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::S))
					camera.Translate(-forward * speed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::Q))
					camera.Translate(left * speed * elapsedTime);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::D))
					camera.Translate(-left * speed * elapsedTime);

				// En revanche, ici la hauteur est toujours la même, peu importe notre orientation
				if (NzKeyboard::IsKeyPressed(NzKeyboard::Space))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);

				if (NzKeyboard::IsKeyPressed(NzKeyboard::LControl))
					camera.Translate(up * speed * elapsedTime, nzCoordSys_Global);
			}

			if (thirdPerson)
			{
				static NzQuaternionf rotDown(NzEulerAnglesf(-35.f, 0.f, 0.f)); // Une rotation pour regarder vers le bas
				camera.SetRotation(drfreak.GetDerivedRotation() * rotDown);

				camera.SetTranslation(drfreak.GetDerivedTranslation() + camera.GetDerivedRotation() * NzVector3f(0.f, 30.f, 50.f));
			}

			// Animation
			if (!paused)
			{
				drfreak.Update(elapsedTime);
				hellknight.Update(elapsedTime);
				/*AnimateModel(hellknight, elapsedTime);
				hellknight.mesh.GetSkeleton()->GetJoint("luparm")->SetScale(2.f);
				hellknight.mesh.Skin(hellknight.mesh.GetSkeleton());*/
			}

			updateClock.Restart();
		}

		NzRenderer::SetMatrix(nzMatrixType_View, NzMatrix4f::LookAt(camera.GetDerivedTranslation(), camera.GetDerivedTranslation() + camera.GetDerivedRotation() * NzVector3f::Forward()));

		NzVector3f translation = drfreak.GetTranslation();
		translation.y = -drfreak.GetMesh()->GetAABB().GetMinimum().y;
		drfreak.SetTranslation(translation);

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

		// On élimine les faces qu'on ne voit pas
		NzRenderer::Enable(nzRendererParameter_FaceCulling, true);

		DrawModel(drfreak);

		if (drawHellknight)
			DrawModel(hellknight);

		if (drawSkeleton)
		{
			NzDebugDrawer::SetDepthTest(false);
			NzDebugDrawer::SetPrimaryColor(NzColor::Blue);
			NzDebugDrawer::Draw(hellknight.GetSkeleton());
		}

		if (drawAabb)
		{
/*			NzDebugDrawer::SetDepthTest(true);
			NzDebugDrawer::SetPrimaryColor(NzColor::Red);
			NzDebugDrawer::Draw(hellknight.mesh.GetAABB());*/

			NzAxisAlignedBox aabb(drfreak.GetMesh()->GetAABB());
			aabb.Transform(drfreak.GetTransformMatrix());

			NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Translate(drfreak.GetDerivedTranslation()));
			NzDebugDrawer::SetPrimaryColor(NzColor::Red);
			NzDebugDrawer::Draw(aabb);

			NzRenderer::SetMatrix(nzMatrixType_World, drfreak.GetTransformMatrix());
			NzDebugDrawer::SetPrimaryColor(NzColor::Blue);
			NzDebugDrawer::Draw(drfreak.GetMesh()->GetAABB());
		}

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

	NzDebugDrawer::Uninitialize();

    return EXIT_SUCCESS;
}

bool CreateCheckerMaterial(NzMaterial* material)
{
	NzImage image;
	// Nous crééons une image 2D, au format RGBA8 de dimensions 128*128 (8 blocs de 16 pixels de côté)
	if (!image.Create(nzImageType_2D, nzPixelFormat_RGBA8, 8*16, 8*16))
	{
		// Ne devrait pas arriver (La création d'une image ne peut échouer que si l'un des argument est incorrect)
		std::cout << "Failed to create image, this means that a bug has been found in Nazara" << std::endl;
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

	NzTexture* texture = new NzTexture;
	if (!texture->LoadFromImage(image)) // Nous créons notre texture depuis notre image
	{
		// Nous n'avons vraiment pas beaucoup de chance..
		std::cout << "Failed to load image" << std::endl;
		return false;
	}

	texture->SetAnisotropyLevel(NzRenderer::GetMaxAnisotropyLevel()); // Un filtrage anisotropique pour la texture
	texture->SetWrapMode(nzTextureWrap_Repeat); // Si les coordonnées de texture dépassent 1.f, la texture sera répétée

	material->SetDiffuseMap(texture);

	texture->SetPersistent(false);

	return true;
}

bool CreateFloorModel(NzModel* model)
{
	// Cette fonction créé un mesh statique simpliste pour servir de sol
	NzMesh* mesh = new NzMesh;
	// Nous créons un mesh statique
	if (!mesh->CreateStatic())
	{
		// L'échec est techniquement impossible mais le moteur étant en constante évolution ...
		std::cout << "Failed to create mesh" << std::endl;
		delete mesh;

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
	NzVertexBuffer* buffer = new NzVertexBuffer(declaration, 4, nzBufferStorage_Hardware, nzBufferUsage_Static);

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
	if (!subMesh->Create(buffer))
	{
		std::cout << "Failed to create subMesh" << std::endl;
		return false;
	}

	subMesh->SetMaterialIndex(0);
	subMesh->SetPrimitiveType(nzPrimitiveType_TriangleStrip);

	// On ajoute le submesh au mesh
	mesh->AddSubMesh(subMesh);
	mesh->SetMaterialCount(1);

	// Nos ressources sont notifiées utilisées par le mesh et le submesh, nous pouvons les rendre éphèmères.
	// Les ressources seront donc automatiquement libérées lorsqu'elles ne seront plus référencées par une classe
	buffer->SetPersistent(false);
	declaration->SetPersistent(false);
	subMesh->SetPersistent(false); // Pour le submesh, c'est déjà le comportement par défaut

	NzModelParameters params;
	params.loadAnimation = false;
	params.loadMaterials = false;

	model->SetMesh(mesh, params);
	mesh->SetPersistent(false);

	NzMaterial* material = new NzMaterial;
	CreateCheckerMaterial(material);

	model->SetMaterial(0, material);
	material->SetPersistent(false);

	return true;
}

void DrawModel(const NzModel& model)
{
	// La matrice world est celle qui représente les transformations du modèle
	NzRenderer::SetMatrix(nzMatrixType_World, model.GetTransformMatrix());

	// Un mesh est divisé en plusieurs submeshes
	unsigned int subMeshCount = model.GetMesh()->GetSubMeshCount();
	for (unsigned int i = 0; i < subMeshCount; ++i)
	{
		// On récupère le submesh
		const NzSubMesh* subMesh = model.GetMesh()->GetSubMesh(i);

		NzRenderer::ApplyMaterial(model.GetMaterial(i));

		NzRenderer::SetVertexBuffer(subMesh->GetVertexBuffer());

		// On fait le rendu
		const NzIndexBuffer* indexBuffer = subMesh->GetIndexBuffer();
		if (indexBuffer)
		{
			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::DrawIndexedPrimitives(subMesh->GetPrimitiveType(), 0, indexBuffer->GetIndexCount());
		}
		else
			NzRenderer::DrawPrimitives(subMesh->GetPrimitiveType(), 0, subMesh->GetVertexCount());
	}
}
