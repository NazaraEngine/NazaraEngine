/*
** Nz::HardwareInfo - Récupération des caractéristiques de l'ordinateur
** Prérequis: Aucun
** Utilisation du noyau et du module de rendu
** Présente:
** - Récupération des informations sur le processeur
** - Récupération des informations sur la carte graphique
*/

#include <Nazara/Core/File.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <cstdio>
#include <iostream>
#include <sstream>

void printCap(std::ostream& o, const std::string& cap, bool b);

int main()
{
	std::cout << "Initialisation en cours...";

	// On va afficher le tout via un ostringstream, pour écrire dans la console et aussi dans un fichier
	std::ostringstream oss;

	oss << "--Processeur--" << std::endl;
	// Plutôt que d'initialiser le Renderer de Nazara, nous initialisons les deux classes utilisées ici
	// Elles sont compatibles avec NzInitialiser et seront donc libérées automatiquement
	// Cela permet d'avoir une initialisation plus rapide et un coût en mémoire moindre
	Nz::Initializer<Nz::HardwareInfo> hardwareInfo;
	if (hardwareInfo)
	{
		// On commence par les informations sur le processeur, Nazara en récupère trois caractéristiques:
		// 1) La "brand string", qui est une chaîne de 48 caractères identifiant le processeur
		// 2) Le concepteur du processeur, accessible via une énumération (GetProcessorVendor) ou une chaîne de caractère (GetProcessorVendorName)
		// 3) Le nombre de processeurs logique, alias bien souvent le nombre de coeurs (logiques), cette valeur est renvoyée par l'OS (Le SMT multiplie donc la valeur réelle)
		oss << "Identification: " << Nz::HardwareInfo::GetProcessorBrandString() << std::endl;
		oss << "Concepteur: " << Nz::HardwareInfo::GetProcessorVendorName() << std::endl;
		oss << "Nombre de coeurs logiques: " << Nz::HardwareInfo::GetProcessorCount() << std::endl;
		oss << std::endl;

		// Ensuite, Nazara récupère les capacités du processeur, dont des jeux d'extensions supplémentaires
		oss << "Rapport des capacites: " << std::endl;// Pas d'accent car écriture dans un fichier (et on ne va pas s'embêter avec ça)
		printCap(oss, "-64bits", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_x64));
		printCap(oss, "-AVX", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_AVX));
		printCap(oss, "-FMA3", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_FMA3));
		printCap(oss, "-FMA4", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_FMA4));
		printCap(oss, "-MMX", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_MMX));
		printCap(oss, "-SSE", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSE));
		printCap(oss, "-SSE2", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSE2));
		printCap(oss, "-SSE3", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSE3));
		printCap(oss, "-SSSE3", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSSE3));
		printCap(oss, "-SSE4.1", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSE41));
		printCap(oss, "-SSE4.2", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSE42));
		printCap(oss, "-SSE4.a", Nz::HardwareInfo::HasCapability(Nz::ProcessorCap_SSE4a));
	}
	else
		oss << "Impossible de retrouver les informations du processeur" << std::endl;

	std::cout << "\r                          "; // On efface le message d'initialisation
	std::cout << '\r'; // Et on place déjà le caractère pour revenir sur la même ligne (Pour ne pas avoir un saut inutile)

	std::cout << oss.str() << std::endl;

	Nz::File reportFile("HardwareInfo.txt");
	if (reportFile.Open(Nz::OpenMode_Text | Nz::OpenMode_Truncate | Nz::OpenMode_WriteOnly))
	{
		reportFile.Write(oss.str()); // Conversion implicite en std::string
		reportFile.Close();

		char accentAigu = static_cast<char>(130); // C'est crade, mais ça marche chez 95% des Windowsiens
		std::cout << "Un fichier (RapportNz::HardwareInfo.txt) contenant le rapport a " << accentAigu << 't' << accentAigu << " cr" << accentAigu << accentAigu << std::endl;
	}
	else
		std::cout << "Impossible de sauvegarder le rapport" << std::endl;

	std::getchar();

	return 0;
}

void printCap(std::ostream& o, const std::string& cap, bool b)
{
	if (b)
		o << cap << ": Oui" << std::endl;
	else
		o << cap << ": Non" << std::endl;
}
