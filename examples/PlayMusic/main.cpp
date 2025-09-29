/*
** PlayMusic - Example on playing a sound using streaming (doesn't load all the file in memory, only the played part) with Nz::Music
*/

#include <Nazara/Audio2.hpp>
#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/SignalHandlerAppComponent.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Application<Nz::Audio2> app(argc, argv);
	app.AddComponent<Nz::SignalHandlerAppComponent>();

	std::shared_ptr<Nz::AudioEngine> audioEngine = Nz::Audio2::Instance()->OpenPlaybackEngine();

	std::shared_ptr<Nz::SoundStream> soundBuffer = Nz::SoundStream::OpenFromFile(resourceDir / "Audio/file_example_MP3_700KB.mp3");

	Nz::Sound sound(Nz::Sound::Config{ .engine = audioEngine.get(), .source = soundBuffer });
	sound.Play();

	std::cout << "Playing sound..." << std::endl;

	app.AddUpdaterFunc([&]
	{
		if (!sound.IsPlaying())
			app.Quit();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	});

	return app.Run();
}
