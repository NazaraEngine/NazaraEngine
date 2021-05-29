/*
** DopplerEffect - Introduction à la lecture de son spatialisé (+ démonstration de l'effet doppler)
** Prérequis: Aucun
** Utilisation du noyau et du module audio
** Présente:
** - Chargement, lecture et positionnement d'un son
** - Gestion basique d'une horloge
** - Gestion basique de position 3D
*/

#include <Nazara/Audio.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	std::filesystem::path resourceDir = "resources";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
		resourceDir = ".." / resourceDir;

	Nz::Modules<Nz::Audio> audio;

	Nz::SoundStreamParams streamParams;
	streamParams.forceMono = false;

	Nz::Music music;
	if (!music.OpenFromFile(resourceDir / "file_example_MP3_700KB.mp3", streamParams))
	{
		std::cout << "Failed to load sound" << std::endl;
		std::getchar();
		return EXIT_FAILURE;
	}

	music.Play();

	std::cout << "Playing sound..." << std::endl;

	while (music.IsPlaying())
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return EXIT_SUCCESS;
}
