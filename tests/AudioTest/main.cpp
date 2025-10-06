#include <Nazara/Core.hpp>
#include <Nazara/Audio.hpp>
#include <Nazara/Audio/AudioEffect.hpp>
#include <iostream>

int main()
{
	Nz::Application<Nz::Audio> app;

	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	std::vector<Nz::AudioDeviceInfo> devices = Nz::Audio::Instance()->QueryDevices();
	for (const auto& deviceInfo : devices)
	{
		std::cout << std::string_view(&deviceInfo.deviceName[0]) << "\n";
		std::cout << " - type: " << ((deviceInfo.deviceType == Nz::AudioDeviceType::Playback) ? "playback" : "capture") << "\n";
		std::cout << " - default: " << ((deviceInfo.isDefault) ? "yes" : "no") << "\n";
	}
	std::cout << std::flush;

	std::shared_ptr<Nz::SoundBuffer> soundBuffer = Nz::SoundBuffer::LoadFromFile(resourceDir / "Audio/file_example_MP3_700KB.mp3");

	std::shared_ptr<Nz::AudioEngine> audioEngine = Nz::Audio::Instance()->OpenPlaybackEngine();

	Nz::AudioReverbEffect reverbEffect({ .engine = audioEngine.get() });
	reverbEffect.AttachOutputBus(0, audioEngine->GetEndpoint(), 0);

	Nz::Sound sound(Nz::Sound::Config{ .engine = audioEngine.get(), .source = soundBuffer, .outputNode = &reverbEffect });
	sound.Play();

	std::getchar();

	return 0;
}
