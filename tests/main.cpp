#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Shader/Shader.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <glslang/Public/ShaderLang.h>

int main(int argc, char* argv[])
{
	Nz::Modules<Nz::Audio, Nz::Network, Nz::Physics2D, Nz::Shader, Nz::Utility> nazaza;

	if (!glslang::InitializeProcess())
		return EXIT_FAILURE;

	int result = Catch::Session().run(argc, argv);

	glslang::FinalizeProcess();

	return result;
}
