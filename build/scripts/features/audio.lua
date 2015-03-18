return {
    Title = "Module audio",
	LibName = "NazaraAudio",
	Description = "Module permettant de charger et jouer des sons, ainsi que d'accéder au microphone",
	RequiredPortability = Feature.Windows + Feature.POSIX,

	Features = 
	{
		{
			Title = "Gestion des émetteurs de sons (SoundEmitter)",
			Description = "Classe de base dont héritent les sources sonores (Music, Sound)",
			Features =
			{
				"Positionnement 3D (spatialisation) si désiré",
				"Configuration de l'atténuation",
				"Prise en charge de l'effet Doppler (vitesse du son)",
				"Altération du ton (pitch)"
			}
		},
		{
			Title = "Gestion des tampons sonores (SoundBuffer)",
			Description = "Tampon (buffer) stockant les échantillons (samples) décompressés, chargés à partir de fichiers sonores.",
			Features =
			{
				{
					Title = "Mixage mono",
					Description = "Permet de mixer les différents canaux en un seul (mono), permettant la spatialisation.",
					Note = "Le mixage mono n'est actuellement possible qu'au chargement du tampon depuis un fichier",
					Progress = 90
				}
			}
		},
		{
			Title = "Gestion des flux sonores (SoundStream)",
			Description = "Interface permettant de définir un flux sonore de source indéfinie (fichier, réseau, etc.)"
		},
		{
			Title = "Sons (Sound)",
			Description = "Classe permettant de jouer un tampon sonore",
			Features =
			{
				"Bouclage",
				"Mise en pause",
				"Récupération/positionnement de l'indice de lecture (offset)"
			}
		},
		{
			Title = "Sons streamés (Music)",
			Description = "Classe permettant de jouer un son depuis un flux quelconque (SoundStream)",
			Features =
			{
				"Streaming depuis un fichier sonore (+ mixage mono si demandé)",
				"Bouclage",
				"Mise en pause",
				{Title = "Mixage mono", Note = "Le mixage mono devrait être possible au niveau du son lui-même plutôt qu'au niveau du loader", Progress = 0},
				{Title = "Récupération/positionnement de l'indice de lecture (offset)", Progress = 0}
			}
		},
		{
			Title = "Configuration globale (Audio)",
			Description = "Classe permettant de régler les différents paramètres généraux de l'environnement sonore",
			Features =
			{
				"Positionnement/Orientation du listener",
				"Réglage du volume général",
				"Réglage de la vitesse du son/facteur Doppler"
			}
		},
		{
			Title = "Microphone",
			Description = "Classe permettant l'accès aux microphones connectés à l'ordinateur",
			Progress = 0
		},
		{
			Title = "Environnement (EFX)",
			Description = "Prise en charge de l'environnement",
			Progress = 0
		},
		{
			Title = "Formats supportés (chargement/streaming)",
			Description = "Liste des formats de fichiers supportés par le loader inclus par le module (libsndfile)",
			Note = "Le populaire format MP3 n'est pas supporté pour des raisons de royalties (mais la flexibilité du moteur vous permet de rajouter votre propre loader MP3)",
			Features = 
			{
				"aiff", "au", "avr", "caf", "flac", "htk", "ircam", "mat4", "mat5", "mpc2k", "nist","ogg", "pvf", "raw", "rf64", "sd2", "sds", "svx", "voc", "w64", "wav", "wve"
			}
		}
    }
}