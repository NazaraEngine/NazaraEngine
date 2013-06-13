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
#include <Nazara/Utility/Keyboard.hpp>
#include <iostream>

int main()
{
	// NzKeyboard ne nécessite pas l'initialisation du module Utilitaire
	NzInitializer<NzAudio> audio;
	if (!audio)
	{
		std::cout << "Failed to initialize audio module" << std::endl;
		std::getchar();
		return 1;
	}

	NzSound sound;
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
	sound.SetPosition(NzVector3f::Left()*50.f + NzVector3f::Forward()*5.f);

	// Et possède une vitesse de 10 par seconde vers la droite
	sound.SetVelocity(NzVector3f::Left()*-10.f);

	// On joue le son
	sound.Play();

	// La boucle du programme (Pour déplacer le son)
	NzClock clock;
	while (sound.GetStatus() == nzSoundStatus_Playing)
	{
		// Comme le son se joue dans un thread séparé, on peut mettre en pause le principal régulièrement
		int sleepTime = 1000/60 - clock.GetMilliseconds(); // 60 FPS

		if (sleepTime > 0)
			NzThread::Sleep(sleepTime);

		// On bouge la source du son en fonction du temps depuis chaque mise à jour
		NzVector3f pos = sound.GetPosition() + sound.GetVelocity()*clock.GetSeconds();
		sound.SetPosition(pos);

		std::cout << "Sound position: " << pos << std::endl;

		// Si la position de la source atteint une certaine position, ou si l'utilisateur appuie sur echap
		if (pos.x > NzVector3f::Left().x*-50.f || NzKeyboard::IsKeyPressed(NzKeyboard::Escape))
			sound.Stop(); // On arrête le son (Stoppant également la boucle)

		clock.Restart();
	}

	return 0;
}
