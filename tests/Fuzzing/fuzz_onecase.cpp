#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/AbstractLogger.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Shader/Shader.hpp>

int fuzz_one(std::string const &s)
{
    const char* arguments[] = { ("--input-file " + s).c_str() };
    int result = Catch::Session().run(1, arguments);
    return result;
}

int main(int argc, char* argv[])
{
	Nz::Modules<Nz::Audio, Nz::Network, Nz::Physics2D, Nz::Shader> nazaza;

	std::string s((std::istreambuf_iterator<char>(std::cin)),
	               (std::istreambuf_iterator<char>()));

    int result = fuzz_one(s);

	return result;
}
