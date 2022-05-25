#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Utility/Utility.hpp>

int main(int argc, char* argv[])
{
	Nz::Modules<Nz::Audio, Nz::Network, Nz::Physics2D, Nz::Utility> nazaza;

	return Catch::Session().run(argc, argv);
}
