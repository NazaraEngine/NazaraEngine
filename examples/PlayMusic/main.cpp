/*
** PlayMusic - Example on playing a sound using streaming (doesn't load all the file in memory, only the played part) with Nz::Music
*/

#include <Nazara/Audio.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Utility/BasicMainloop.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	try
	{

		std::filesystem::path resourceDir = "assets/examples";
		if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
			resourceDir = "../.." / resourceDir;

		Nz::Modules<Nz::Audio> audio;

		Nz::SoundStreamParams streamParams;
		streamParams.forceMono = false;

		Nz::Music music;
		if (!music.OpenFromFile(resourceDir / "Audio/file_example_MP3_700KB.mp3", streamParams))
		{
			std::cout << "Failed to load sound" << std::endl;
			std::getchar();
			return EXIT_FAILURE;
		}

		std::getchar();

		music.Play();

		std::cout << "Playing sound..." << std::endl;

		Nz::Window window;

		Nz::Clock clock;
		Nz::BasicMainloop(window, [&] {
		});

		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
