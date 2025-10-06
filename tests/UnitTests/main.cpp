#define CATCH_CONFIG_RUNNER
#include <catch2/catch_session.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/TextRenderer/TextRenderer.hpp>

int main(int argc, char* argv[])
{
	Nz::Audio::Config audioConfig{ .noAudio = true };
	Nz::Modules<Nz::Audio, Nz::Network, Nz::Physics2D, Nz::TextRenderer> nazaza(audioConfig);

	return Catch::Session().run(argc, argv);
}
