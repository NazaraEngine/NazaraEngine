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
#include <Nazara/Core/Thread.hpp> // Thread::Sleep
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Platform/Keyboard.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <iostream>

int main()
{
	// NzKeyboard nécessite l'initialisation du module Utilitaire
	Nz::Initializer<Nz::Audio, Nz::Platform> audio;
	if (!audio)
	{
		std::cout << "Failed to initialize audio module" << std::endl;
		std::getchar();
		return 1;
	}

	Nz::Sound sound;
	if (!sound.LoadFromFile("resources/siren.wav"))
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
	sound.SetPosition(Nz::Vector3f::Left()*50.f + Nz::Vector3f::Forward()*5.f);

	// Et possède une vitesse de 10 par seconde vers la droite
	sound.SetVelocity(Nz::Vector3f::Left()*-10.f);

	// On joue le son
	sound.Play();

	// La boucle du programme (Pour déplacer le son)
	Nz::Clock clock;
	while (sound.GetStatus() == Nz::SoundStatus_Playing)
	{
		// Comme le son se joue dans un thread séparé, on peut mettre en pause le principal régulièrement
		int sleepTime = int(1000/60 - clock.GetMilliseconds()); // 60 FPS

		if (sleepTime > 0)
			Nz::Thread::Sleep(sleepTime);

		// On bouge la source du son en fonction du temps depuis chaque mise à jour
		Nz::Vector3f pos = sound.GetPosition() + sound.GetVelocity()*clock.GetSeconds();
		sound.SetPosition(pos);

		std::cout << "Sound position: " << pos << std::endl;

		// Si la position de la source atteint une certaine position, ou si l'utilisateur appuie sur echap
		if (pos.x > Nz::Vector3f::Left().x*-50.f || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::Escape))
			sound.Stop(); // On arrête le son (Stoppant également la boucle)

		clock.Restart();
	}

	return 0;
}
