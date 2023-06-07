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
#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Core/SignalHandlerAppComponent.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Application<Nz::Audio> app;
	app.AddComponent<Nz::SignalHandlerAppComponent>();

	Nz::Sound sound;
	if (!sound.LoadFromFile(resourceDir / "Audio/siren.wav"))
	{
		std::cout << "Failed to load sound" << std::endl;
		std::getchar();
		return 1;
	}

	std::cout << "Doppler effect demo" << std::endl;
	std::cout << "Press enter to start" << std::endl;

	std::getchar();

	// Make a repeating sound, located to the left (and a bit forward so it doesn't switch from left to right speaker brutally) with a right velocity
	sound.EnableLooping(true);
	sound.SetPosition(Nz::Vector3f::Left() * 50.f + Nz::Vector3f::Forward() * 5.f);
	sound.SetVelocity(Nz::Vector3f::Right() * 10.f);

	sound.Play();

	app.AddUpdaterFunc([&](Nz::Time elapsedTime)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));

		if (sound.GetStatus() != Nz::SoundStatus::Playing)
			app.Quit();

		// Move sound position according to its velocity
		Nz::Vector3f pos = sound.GetPosition() + sound.GetVelocity() * elapsedTime.AsSeconds();
		sound.SetPosition(pos);

		std::cout << "Position: " << pos.x << std::endl;

		// Stop once far enough
		if (pos.x > Nz::Vector3f::Right().x * 50.f)
		{
			sound.Stop();
			app.Quit();
		}
	});

	return app.Run();
}
