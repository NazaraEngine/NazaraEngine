#define CATCH_CONFIG_RUNNER
#include <catch2/catch_session.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkPhysics2D.hpp>
#include <Nazara/Utility/Utility.hpp>

int main(int argc, char* argv[])
{
	Nz::Modules<Nz::Audio, Nz::Network, Nz::ChipmunkPhysics2D, Nz::Utility> nazaza;

	return Catch::Session().run(argc, argv);
}
