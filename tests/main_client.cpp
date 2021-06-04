#define CATCH_CONFIG_RUNNER
#include <Catch/catch.hpp>

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Modules.hpp>
#include <NazaraSDK/ClientApplication.hpp>
#include <NazaraSDK/ClientSdk.hpp>

int main(int argc, char* argv[])
{
	Nz::Modules<Ndk::ClientSdk> nazaza;
	Ndk::ClientApplication app(argc, argv);

	int result = Catch::Session().run(argc, argv);

	return result;
}
