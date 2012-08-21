#include <Nazara/Audio.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Thread.hpp> // Thread::Sleep
#include <Nazara/Utility/Keyboard.hpp>
#include <iostream>

int main()
{
	// NzKeyboard ne nécessite pas l'initialisation d'Utility
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

	// La source du son se situe en (50, 0, 5)
	sound.SetPosition(50, 0, 5);

	// Et possède une vitesse de -10 par seconde sur l'axe X
	sound.SetVelocity(-10, 0, 0);

	// On joue le son
	sound.Play();

	// La boucle du programme (Pour déplacer le son)
	NzClock clock;
	while (sound.GetStatus() == nzSoundStatus_Playing)
	{
		// Comme le son se joue dans un thread séparé, on peut mettre en pause celui-ci régulièrement
		int sleepTime = 1000/60 - clock.GetMilliseconds(); // 60 FPS

		if (sleepTime > 0)
			NzThread::Sleep(sleepTime);

		// On bouge la source du son en fonction du au temps depuis chaque mise à jour
		NzVector3f pos = sound.GetPosition() + sound.GetVelocity()*clock.GetSeconds();
		sound.SetPosition(pos);

		std::cout << "Sound position: " << pos << std::endl;

		// Si la position de la source atteint une certaine position, ou si l'utilisateur appuie sur echap
		if (pos.x < -50.f || NzKeyboard::IsKeyPressed(NzKeyboard::Escape))
			sound.Stop(); // On arrête le son (Stoppant également la boucle)

		clock.Restart();
	}

	return 0;
}
