#define CATCH_CONFIG_RUNNER
#include <Catch/catch.hpp>

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Modules.hpp>
#include <NazaraSDK/Application.hpp>
#include <NazaraSDK/Sdk.hpp>

int main(int argc, char* argv[])
{
	Nz::Modules<Ndk::Sdk> nazaza;
	Ndk::Application app(argc, argv);

	Nz::Log::GetLogger()->EnableStdReplication(false);

	int result = Catch::Session().run(argc, argv);

	return result;
}
