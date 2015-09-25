/*
** HardwareInfo - Récupération des caractéristiques de l'ordinateur
** Prérequis: Aucun
** Utilisation du noyau et du module de rendu
** Présente:
** - Récupération des informations sur le processeur
** - Récupération des informations sur la carte graphique
*/

#include <Nazara/Core/File.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <cstdio>
#include <iostream>
#include <sstream>

void printCap(std::ostream& o, const String& cap, bool b);

int main()
{
	std::cout << "Initialisation en cours...";

	// On va afficher le tout via un ostringstream, pour écrire dans la console et aussi dans un fichier
	std::ostringstream oss;

	oss << "--Processeur--" << std::endl;
	// Plutôt que d'initialiser le Renderer de Nazara, nous initialisons les deux classes utilisées ici
	// Elles sont compatibles avec NzInitialiser et seront donc libérées automatiquement
	// Cela permet d'avoir une initialisation plus rapide et un coût en mémoire moindre
	Initializer<HardwareInfo> hardwareInfo;
	if (hardwareInfo)
	{
		// On commence par les informations sur le processeur, Nazara en récupère trois caractéristiques:
		// 1) La "brand string", qui est une chaîne de 48 caractères identifiant le processeur
		// 2) Le concepteur du processeur, accessible via une énumération (GetProcessorVendor) ou une chaîne de caractère (GetProcessorVendorName)
		// 3) Le nombre de processeurs logique, alias bien souvent le nombre de coeurs (logiques), cette valeur est renvoyée par l'OS (Le SMT multiplie donc la valeur réelle)
		oss << "Identification: " << HardwareInfo::GetProcessorBrandString() << std::endl;
		oss << "Concepteur: " << HardwareInfo::GetProcessorVendorName() << std::endl;
		oss << "Nombre de coeurs logiques: " << HardwareInfo::GetProcessorCount() << std::endl;
		oss << std::endl;

		// Ensuite, Nazara récupère les capacités du processeur, dont des jeux d'extensions supplémentaires
		oss << "Rapport des capacites: " << std::endl;// Pas d'accent car écriture dans un fichier (et on ne va pas s'embêter avec ça)
		printCap(oss, "-64bits", HardwareInfo::HasCapability(ProcessorCap_x64));
		printCap(oss, "-AVX", HardwareInfo::HasCapability(ProcessorCap_AVX));
		printCap(oss, "-FMA3", HardwareInfo::HasCapability(ProcessorCap_FMA3));
		printCap(oss, "-FMA4", HardwareInfo::HasCapability(ProcessorCap_FMA4));
		printCap(oss, "-MMX", HardwareInfo::HasCapability(ProcessorCap_MMX));
		printCap(oss, "-SSE", HardwareInfo::HasCapability(ProcessorCap_SSE));
		printCap(oss, "-SSE2", HardwareInfo::HasCapability(ProcessorCap_SSE2));
		printCap(oss, "-SSE3", HardwareInfo::HasCapability(ProcessorCap_SSE3));
		printCap(oss, "-SSSE3", HardwareInfo::HasCapability(ProcessorCap_SSSE3));
		printCap(oss, "-SSE4.1", HardwareInfo::HasCapability(ProcessorCap_SSE41));
		printCap(oss, "-SSE4.2", HardwareInfo::HasCapability(ProcessorCap_SSE42));
		printCap(oss, "-SSE4.a", HardwareInfo::HasCapability(ProcessorCap_SSE4a));
	}
	else
		oss << "Impossible de retrouver les informations du processeur" << std::endl;

	oss << std::endl << "--Carte graphique--" << std::endl;
	// La classe OpenGL nous donne accès à des informations sur la carte graphique
	// Cependant celle-ci n'est accessible que si le projet est compilé avec NAZARA_RENDERER_OPENGL
	// et que les répertoires d'inclusions donnent accès aux includes d'OpenGL (Cette démo utilisent ceux de Nazara)
	Initializer<OpenGL> openGL;
	if (openGL)
	{
		// Nous récupérons ensuite la version d'OpenGL sous forme d'entier (ex: OpenGL 3.3 donnera 330)
		unsigned int openglVersion = OpenGL::GetVersion();

		// OpenGL nous donne accès à trois informations principales:
		// 1) La chaîne d'identification du driver ("Renderer name")
		// 2) La chaîne d'identification du concepteur ("Vendor name")
		// 3) La version d'OpenGL
		oss << "Identification: " << OpenGL::GetRendererName() << std::endl;
		oss << "Concepteur: " << OpenGL::GetVendorName() << std::endl;
		oss << "Version d'OpenGL: " << openglVersion/100 << '.' << openglVersion%100 << std::endl;
		oss << std::endl;

		// Ainsi qu'un report des capacités de la carte graphique (avec le driver actuel)
		oss << "Rapport des capacites: " << std::endl; // Pas d'accent car écriture dans un fichier (et on ne va pas s'embêter avec ça)
		printCap(oss, "-Calculs 64bits", OpenGL::IsSupported(OpenGLExtension_FP64));
		printCap(oss, "-Compression de textures (s3tc)", OpenGL::IsSupported(OpenGLExtension_TextureCompression_s3tc));
		printCap(oss, "-Filtrage anisotrope", OpenGL::IsSupported(OpenGLExtension_AnisotropicFilter));
		printCap(oss, "-Framebuffer Object", OpenGL::IsSupported(OpenGLExtension_FrameBufferObject));
		printCap(oss, "-Mode debug", OpenGL::IsSupported(OpenGLExtension_DebugOutput));
		printCap(oss, "-Pixelbuffer Object", OpenGL::IsSupported(OpenGLExtension_PixelBufferObject));
		printCap(oss, "-Samplers Object", OpenGL::IsSupported(OpenGLExtension_SamplerObjects));
		printCap(oss, "-Separate shader objects", OpenGL::IsSupported(OpenGLExtension_SeparateShaderObjects));
		printCap(oss, "-Texture array", OpenGL::IsSupported(OpenGLExtension_TextureArray));
		printCap(oss, "-Texture storage", OpenGL::IsSupported(OpenGLExtension_TextureStorage));
		printCap(oss, "-Vertex array objects", OpenGL::IsSupported(OpenGLExtension_VertexArrayObjects));
	}
	else
		oss << "Impossible de retrouver les informations de la carte graphique" << std::endl;

	std::cout << "\r                          "; // On efface le message d'initialisation
	std::cout << '\r'; // Et on place déjà le caractère pour revenir sur la même ligne (Pour ne pas avoir un saut inutile)

	std::cout << oss.str() << std::endl;

	File reportFile("RapportHardwareInfo.txt");
	if (reportFile.Open(OpenMode_Text | OpenMode_Truncate | OpenMode_WriteOnly))
	{
		reportFile.Write(oss.str()); // Conversion implicite en String
		reportFile.Close();

		char accentAigu = static_cast<char>(130); // C'est crade, mais ça marche chez 95% des Windowsiens
		std::cout << "Un fichier (RapportHardwareInfo.txt) contenant le rapport a " << accentAigu << 't' << accentAigu << " cr" << accentAigu << accentAigu << std::endl;
	}
	else
		std::cout << "Impossible de sauvegarder le rapport" << std::endl;

	std::getchar();

	return 0;
}

void printCap(std::ostream& o, const String& cap, bool b)
{
	if (b)
		o << cap << ": Oui" << std::endl;
	else
		o << cap << ": Non" << std::endl;
}
