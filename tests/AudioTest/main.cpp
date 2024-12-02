#include <Nazara/Core.hpp>
#include <Nazara/Audio2.hpp>
#include <Nazara/Audio2/AudioDevice.hpp>
#include <iostream>

int main()
{
	Nz::Application<Nz::Audio2> app;

	std::vector<Nz::AudioDeviceInfo> devices = Nz::Audio2::Instance()->QueryDevices();
	for (const auto& deviceInfo : devices)
	{
		std::cout << std::string_view(&deviceInfo.deviceName[0]) << "\n";
		std::cout << " - type: " << ((deviceInfo.deviceType == Nz::AudioDeviceType::Playback) ? "playback" : "capture") << "\n";
		std::cout << " - default: " << ((deviceInfo.isDefault) ? "yes" : "no") << "\n";
	}
	std::cout << std::flush;

	std::shared_ptr<Nz::AudioDevice> audioDevice = Nz::Audio2::Instance()->OpenPlaybackDevice();
	audioDevice->SetDataCallback([](const Nz::AudioDevice& audioDevice, const void* inputData, void* outputData, Nz::UInt32 frameCount)
	{
		//std::cout << Nz::Format("Data callback (framecount: {})", frameCount) << std::endl;
	});

	audioDevice->Start();

	std::getchar();

	return 0;
}
