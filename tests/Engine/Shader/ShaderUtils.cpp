#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/LangWriter.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <catch2/catch.hpp>
#include <spirv-tools/libspirv.hpp>

void ExpectingGLSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	expectedOutput = Nz::Trim(expectedOutput);

	Nz::GlslWriter writer;

	SECTION("Generating GLSL")
	{
		std::string output = writer.Generate(shader);

		INFO("full GLSL output:\n" << output << "\nexcepted output:\n" << expectedOutput);
		REQUIRE(output.find(expectedOutput) != std::string::npos);
	}
}

void ExpectingNZSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	expectedOutput = Nz::Trim(expectedOutput);

	Nz::LangWriter writer;

	SECTION("Generating NZSL")
	{
		std::string output = writer.Generate(shader);

		INFO("full NZSL output:\n" << output << "\nexcepted output:\n" << expectedOutput);
		REQUIRE(output.find(expectedOutput) != std::string::npos);

		// validate NZSL by recompiling it
		REQUIRE_NOTHROW(Nz::ShaderLang::Parse(output));
	}
}

void ExpectingSpirV(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	expectedOutput = Nz::Trim(expectedOutput);

	Nz::SpirvWriter writer;
	Nz::SpirvPrinter printer;

	Nz::SpirvPrinter::Settings settings;
	settings.printHeader = false;
	settings.printParameters = false;

	SECTION("Generating SPIRV")
	{
		auto spirv = writer.Generate(shader);
		std::string output = printer.Print(spirv.data(), spirv.size(), settings);

		INFO("full SPIRV output:\n" << output << "\nexcepted output:\n" << expectedOutput);
		REQUIRE(output.find(expectedOutput) != std::string::npos);

		// validate SPIRV with libspirv
		spvtools::SpirvTools spirvTools(spv_target_env::SPV_ENV_VULKAN_1_0);
		spirvTools.SetMessageConsumer([&](spv_message_level_t /*level*/, const char* /*source*/, const spv_position_t& /*position*/, const char* message)
		{
			std::string fullSpirv;
			if (!spirvTools.Disassemble(spirv, &fullSpirv))
				fullSpirv = "<failed to disassemble SPIRV>";

			UNSCOPED_INFO(fullSpirv + "\n" + message);
		});

		REQUIRE(spirvTools.Validate(spirv));
	}
}
