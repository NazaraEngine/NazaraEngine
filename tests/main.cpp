#define CATCH_CONFIG_RUNNER
#include <Catch/catch.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Network/Network.hpp>

int main(int argc, char* const argv[])
{
	Nz::Initializer<Nz::Audio, Nz::Core, Nz::Graphics, Nz::Network> modules;

	int result = Catch::Session().run(argc, argv);

	return result;
}
