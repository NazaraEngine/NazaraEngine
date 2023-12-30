#include <Nazara/Core/CommandLineParameters.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <array>

SCENARIO("CommandLineParameters", "[CORE][CommandLineParameters]")
{
	WHEN("Parsing no parameters")
	{
		Nz::CommandLineParameters emptyParams;

		const char* appName = "UnitTests";
		Nz::CommandLineParameters commandLineParameters1 = Nz::CommandLineParameters::Parse(0, static_cast<char**>(nullptr));
		CHECK(commandLineParameters1 == emptyParams);
		CHECK_FALSE(commandLineParameters1 != emptyParams);
		Nz::CommandLineParameters commandLineParameters2 = Nz::CommandLineParameters::Parse(1, &appName);
		CHECK(commandLineParameters2 == emptyParams);
		CHECK_FALSE(commandLineParameters2 != emptyParams);
	}

	WHEN("Parsing simple parameters")
	{
		std::array<const char*, 5> params = {"exec", "--flag", "--param1=value", "--param2 value2", "ignored"};
		Nz::CommandLineParameters commandLineParameters = Nz::CommandLineParameters::Parse(Nz::SafeCast<int>(params.size()), params.data());

		std::string_view value;

		CHECK(commandLineParameters.HasFlag("flag"));
		CHECK_FALSE(commandLineParameters.HasFlag("ignored"));
		CHECK_FALSE(commandLineParameters.GetParameter("param0", nullptr));
		CHECK(commandLineParameters.GetParameter("param1", nullptr));
		CHECK(commandLineParameters.GetParameter("param1", &value));
		CHECK(value == "value");
		CHECK(commandLineParameters.GetParameter("param2", &value));
		CHECK(value == "value2");
	}
}
