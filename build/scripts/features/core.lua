return {
    Title = "Noyau",
	LibName = "NazaraCore",
	Description = "Noyau du moteur, possède les fonctionnalités utilisées partout ailleurs dans le moteur.",
	RequiredPortability = Feature.Windows + Feature.POSIX,

	Features = 
	{
		{
			Title = "Classes utilitaires de base",
			Description = "Classes assurant certaines fonctionnalités de base destinées à aider le programmeur.",
			Features = 
			{
				{
					Title = "Chaînes de caractère unicode (String)",
					Description = "Classe supplantant std::string, rajoutant le support de l'UTF-8 et quelques opérations utiles (rechercher/remplacer, correspondance par motif, ..)."
				},
				{
					Title = "Exécution de code lors de la destruction (CallOnExit)",
					Description = "Permet l'appel d'une fonction/lambda à la destruction de cet objet, en accordance avec le RAII/RRID."
				},
				{
					Title = "Gestion de l'initialisation/libération statique de classes, en accordance avec le RAII/RRID",
					Description = "Permet d'initialiser et de libérer automatiquement des classes possédant des fonctions membres statiques Initialize/Uninitialize, selon un ordre de dépendance, comme par exemple les classes des modules du moteur lui-même."
				},
				{
					Title = "Gestion des couleurs (Color)",
					Description = "Classe permettant de stocker et effectuer des opérations sur des couleurs (conversion, mélange, etc.)."
				},
				{
					Title = "Gestion de primitives, au sens géométrique (Primitive, PrimitiveList)",
					Description = "Structures définissant certaines primitives géométriques (boite, cône, plan, sphère), utiles pour la génération de meshs (de rendu ou physiques)."
				},
				{
					Title = "OffsetOf",
					Description = "Fonction permettant de récupérer à la compilation l'adresse locale (décalage d'octets) d'un membre de n'importe quelle structure/classe."
				},
				{
					Title = "Pointeurs à écart (stride) indépendant (SparsePtr)",
					Description = "Classe se comportant comme un pointeur, à l'exception de l'écart entre deux éléments (opérations d'accès, d'addition et de soustraction) qui est indépendant du type pointé."
				},
				{
					Title = "Stockage et gestion de champs de bits dynamiques (Bitset)",
					Description = "Classe permettant le stockage et la manipulation optimisée d'un nombre élevé et dynamique de bits (=booléens, opérations de comptage, recherche de bits activés, opérations entre champs de bits, etc.)",
					Note = "Il manque des opérateurs de comparaison et une spécialisation de la fonction de hachage",
					Progress = 90
				}
			}
		},
		{
			Title = "Gestion de fichiers et dossiers (File/Directory)",
			Description = "Classes supportant les chemins unicodes et les tailles 64 bits, ainsi que quelques opérations sur les chemins (normalisation, transformation d'un chemin relatif en chemin absolu).",
			Portability = Feature.Windows + Feature.POSIX
		},
		{
			Title = "Gestion de l'endianness",
			Description = "Boutisme, récupération à la compilation ou à l'exécution, possibilité de lire des fichiers en corrigeant le boutisme des objets lors de la lecture."
		},
		{
			Title = "Gestion de bibliothèques dynamiques (DynLib)",
			Description = "Classe permettant de charger à l'exécution des bibliothèques dynamiques (selon la plateforme: .dll/.so/.dynlib) et d'en extraire des fonctions.",
			Portability = Feature.Windows + Feature.POSIX
		},
		{
			Title = "Fonctions de hachage",
			Description = "Possibilité de hacher (calculer une somme de contrôle) de n'importe quelles données, chaîne de caractère ou fichier.",
			Features = 
			{
				{Title = "CRC16", Progress = 0},
				"CRC32",
				"Fletcher16",
				{Title = "Fletcher32", Progress = 0},
				"MD5",
				"SHA-1",
				"SHA-224",
				"SHA-256",
				"SHA-384",
				"SHA-512",
				"Whirlpool"
			}
		},
		{
			Title = "Gestion de la mémoire",
			Description = "Set de fonctionnalités permettant d'aider le programmeur à gérer la mémoire de son programme.",
			Features =
			{
				{
					Title = "MemoryManager",
					Description = "Classe permettant de suivre les allocations de mémoire et éventuels leaks, peut suivre automatiquement chaque allocation du programme."
				},
				{
					Title = "MemoryPool",
					Description = "Classe permettant d'allouer/libérer des blocs de mémoire de taille prédéfinie à coût constant."
				}
			}
		},
		{
			Title = "Gestion de l'exécution concurrentielle",
			Description = "Set de fonctionnalités liées à l'exécution et la synchronisation entre processus légers (threads) sans dépendance sur la bibliothèque standard.",
			Features =
			{
				{
					Title = "Thread",
					Description = "Classe permettant d'appeler une fonction (avec passage d'arguments) dans un thread, supporte une interface similaire à std::thread.",
					Note = "Il n'est pas encore possible de récupérer le Thread::Id courant",
					Progress = 95,
					Portability = Feature.Windows + Feature.POSIX
				},
				{
					Title = "Mutex",
					Description = "Primitive de synchronisation binaire (exclusion mutuelle).",
					Portability = Feature.Windows + Feature.POSIX
				},
				{
					Title = "Semaphore",
					Description = "Primitive de synchronisation à N concurrents simultanés.",
					Portability = Feature.Windows + Feature.POSIX
				},
				{
					Title = "ConditionVariable",
					Description = "Primitive de synchronisation à signaux.",
					Portability = Feature.Windows + Feature.POSIX
				},
				{
					Title = "LockGuard",
					Description = "Classe utilitaire permettant de verrouiller/déverrouiller une mutex en accordance avec le RAII/RRID."
				},
				{
					Title = "TaskScheduler",
					Description = "Classe permettant de répartir X tâches sur Y workers de façon efficace.",
					Portability = Feature.Windows + Feature.POSIX
				}
			}
		},
		{
			Title = "Gestion du temps",
			Description = "Classes/fonctions permettant de gérer le temps dans un programme.",
			Features =
			{
				{
					Title = "Récupération du temps en millisecondes ou en microsecondes",
					Description = "Permet de récupérer le nombre de millisecondes ou de microsecondes (si supporté par le système) écoulées depuis un référentiel constant (référentiel indéterminé).",
					Portability = Feature.Windows + Feature.POSIX
				},
				{
					Title = "Clock",
					Description = "Classe chronomètre permettant de mesurer le temps écoulé depuis un évènement précis, supporte la mise en pause."
				}
			}
		},
		{
			Title = "Gestion de compteurs de référence",
			Description = "Set de classes permettant de gérer des pointeurs intelligents partagés intrusifs.",
		},
		{
			Title = "Gestion de liste de paramètres",
			Description = "Classe gérant une liste de paramètres nommées à type variable.",
		},
		{
			Title = "Gestion de ressources (chargement/gestion)",
			Description = "Set de classes s'occupant du chargement et de la gestion de certaines classes spéciales, appartenant au groupe des ressources (ex: chargée depuis un fichier).",
			Features = 
			{
				{
					Title = "ResourceLibrary",
					Description = "Classe template chargée du stockage de ressources de façon nommée."
				},
				{
					Title = "ResourceLoader",
					Description = "Classe template chargée de stocker et gérer des loaders (fonctions s'occupant de transformer un flux de données en ressource)."
				},
				{
					Title = "ResourceManager",
					Description = "Classe template chargée de charger et mettre en cache des ressources depuis un chemin de fichier."
				}
			}
		},
		{
			Title = "Gestion des plugins",
			Description = "Capacité d'extension du moteur à l'aide de bibliothèques dynamiques conçues dans cette optique.",
			Note = "Nécessite une révision, permettant aux plugins de se signaler (nom, version)",
			Progress = 80
		},
		{
			Title = "Gestion de l'empaquetage bidimensionnel",
			Description = "Algorithmes permettant de placer des rectangles 2D arbitraires dans un espace prédéfini de façon à minimiser la perte d'espace.",
			Note = "Algorithme Guillotine terminé, algorithme Skyline manquant",
			Progress = 60
		},
		{
			Title = "Gestion de l'Unicode",
			Description = "Récupération d'informations sur les caractères Unicode (si les données Unicode sont incorporées au programme).",
			Note = "Il manque l'intégration des UnicodeData dans le moteur",
			Progress = 20
		},
		{
			Title = "Récupération d'informations sur le hardware",
			Description = "Classe permettant, si la plateforme le supporte, de récupérer des informations utiles sur le matériel: nom et fabricant du processeur, nombre de coeurs, support d'un set d'extension (exemple: SSE), quantité de mémoire vive, exécution de l'instruction CPUID.",
			Progress = 100,
			Portability = Feature.Windows + Feature.POSIX
		}
    }
}