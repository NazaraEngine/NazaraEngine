#define CATCH_CONFIG_RUNNER
#include <Catch/catch.hpp>

#include <NDK/Application.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Network/Network.hpp>

int main(int argc, char* argv[])
{
	Ndk::Application application(argc, argv);
	Nz::Initializer<Nz::Network> modules;

	Nz::Log::GetLogger()->EnableStdReplication(false);

	int result = Catch::Session().run(argc, argv);

	return result;
}
