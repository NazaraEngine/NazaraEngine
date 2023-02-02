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
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Platform/Window.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Application<Nz::Audio> app;

	Nz::Sound sound;
	if (!sound.LoadFromFile(resourceDir / "Audio/siren.wav"))
	{
		std::cout << "Failed to load sound" << std::endl;
		std::getchar();
		return 1;
	}

	std::cout << "Demonstration de l'effet doppler avec Nazara" << std::endl;
	std::cout << "Appuyez sur entree pour demarrer" << std::endl;
	std::cout << "Appuyez sur echap pour arreter" << std::endl;

	std::getchar();

	// On fait en sorte de répéter le son
	sound.EnableLooping(true);

	// La source du son se situe vers la gauche (Et un peu en avant)
	sound.SetPosition(Nz::Vector3f::Left() * 50.f + Nz::Vector3f::Forward() * 5.f);

	// Et possède une vitesse de 10 par seconde vers la droite
	sound.SetVelocity(Nz::Vector3f::Right() * 10.f);

	// On joue le son
	sound.Play();

	Nz::MillisecondClock clock;
	app.AddUpdater([&](Nz::Time elapsedTime)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));

		if (sound.GetStatus() != Nz::SoundStatus::Playing)
		{
			// On arrête le son et l'application
			sound.Stop();
			app.Quit();
		}

		// On bouge la source du son en fonction du temps depuis chaque mise à jour
		Nz::Vector3f pos = sound.GetPosition() + sound.GetVelocity() * clock.Restart().AsSeconds();
		sound.SetPosition(pos);

		std::cout << "Position: " << pos.x << std::endl;

		// Si la position de la source atteint une certaine position, ou si l'utilisateur appuie sur echap
		if (pos.x > Nz::Vector3f::Right().x * 50.f || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Escape))
		{
			sound.Stop();
			app.Quit();
		}
	});

	return app.Run();
}
