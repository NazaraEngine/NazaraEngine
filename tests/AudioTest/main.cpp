#include <Nazara/Core.hpp>
#include <Nazara/Audio2.hpp>
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

	return 0;
}
