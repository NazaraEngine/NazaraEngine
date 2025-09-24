#include <Nazara/Core.hpp>
#include <Nazara/Audio2.hpp>
#include <Nazara/Audio2/AudioDevice.hpp>
#include <Nazara/Audio2/Sound.hpp>
#include <iostream>

int main()
{
	Nz::Application<Nz::Audio2> app;

	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	std::vector<Nz::AudioDeviceInfo> devices = Nz::Audio2::Instance()->QueryDevices();
	for (const auto& deviceInfo : devices)
	{
		std::cout << std::string_view(&deviceInfo.deviceName[0]) << "\n";
		std::cout << " - type: " << ((deviceInfo.deviceType == Nz::AudioDeviceType::Playback) ? "playback" : "capture") << "\n";
		std::cout << " - default: " << ((deviceInfo.isDefault) ? "yes" : "no") << "\n";
	}
	std::cout << std::flush;

	std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(resourceDir / "Audio/file_example_MP3_700KB.mp3");

	std::shared_ptr<Nz::AudioEngine> audioEngine = Nz::Audio2::Instance()->OpenPlaybackEngine();

	Nz::Sound sound(Nz::Sound::Config{ .source = soundBuffer, .engine = audioEngine.get()});
	sound.Play();

	std::getchar();

	return 0;
}
