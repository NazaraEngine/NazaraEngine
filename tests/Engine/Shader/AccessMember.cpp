#include <Nazara/Core/File.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/ShaderAst.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <Catch/catch.hpp>
#include <cctype>

std::string_view Trim(std::string_view str)
{
	while (!str.empty() && std::isspace(str.front()))
		str.remove_prefix(1);

	while (!str.empty() && std::isspace(str.back()))
		str.remove_suffix(1);

	return str;
}

void ExpectingGLSL(const Nz::ShaderAst& shader, std::string_view expectedOutput)
{
	Nz::GlslWriter writer;

	std::string output = writer.Generate(shader);
	std::size_t funcOffset = output.find("void main()");
	std::string_view subset = Trim(output).substr(funcOffset);
	expectedOutput = Trim(expectedOutput);

	REQUIRE(subset == expectedOutput);
}

void ExpectingSpirV(const Nz::ShaderAst& shader, std::string_view expectedOutput)
{
	Nz::SpirvWriter writer;
	auto spirv = writer.Generate(shader);

	Nz::SpirvPrinter printer;

	Nz::SpirvPrinter::Settings settings;
	settings.printHeader = false;
	settings.printParameters = false;

	std::string output = printer.Print(spirv.data(), spirv.size(), settings);
	std::size_t funcOffset = output.find("OpFunction");
	std::string_view subset = Trim(output).substr(funcOffset);
	expectedOutput = Trim(expectedOutput);

	REQUIRE(subset == expectedOutput);
}

SCENARIO("Shader generation", "[Shader]")
{
	SECTION("Nested member loading")
	{
		Nz::ShaderAst baseShader(Nz::ShaderStageType::Vertex);
		baseShader.AddStruct("innerStruct", {
			{
				"field",
				Nz::ShaderNodes::BasicType::Float3
			}
		});
		
		baseShader.AddStruct("outerStruct", {
			{
				"s",
				"innerStruct"
			}
		});

		baseShader.AddUniform("ubo", "outerStruct");
		baseShader.AddOutput("result", Nz::ShaderNodes::BasicType::Float1);

		SECTION("Nested AccessMember")
		{
			Nz::ShaderAst shader = baseShader;

			auto uniform = Nz::ShaderBuilder::Uniform("ubo", "outerStruct");
			auto output = Nz::ShaderBuilder::Output("result", Nz::ShaderNodes::BasicType::Float1);

			auto access = Nz::ShaderBuilder::Swizzle(Nz::ShaderBuilder::AccessMember(Nz::ShaderBuilder::AccessMember(Nz::ShaderBuilder::Identifier(uniform), 0, "innerStruct"), 0, Nz::ShaderNodes::BasicType::Float3), Nz::ShaderNodes::SwizzleComponent::Third);
			auto assign = Nz::ShaderBuilder::Assign(Nz::ShaderBuilder::Identifier(output), access);

			shader.AddFunction("main", Nz::ShaderBuilder::ExprStatement(assign));

			SECTION("Generating GLSL")
			{
				ExpectingGLSL(shader, R"(
void main()
{
	result = ubo.s.field.z;
}
)");
			}
			SECTION("Generating Spir-V")
			{
				ExpectingSpirV(shader, R"(
OpFunction
OpLabel
OpAccessChain
OpAccessChain
OpLoad
OpCompositeExtract
OpStore
OpReturn
OpFunctionEnd)");
			}
		}

		SECTION("AccessMember with multiples fields")
		{
			Nz::ShaderAst shader = baseShader;

			auto uniform = Nz::ShaderBuilder::Uniform("ubo", "outerStruct");
			auto output = Nz::ShaderBuilder::Output("result", Nz::ShaderNodes::BasicType::Float1);

			auto access = Nz::ShaderBuilder::Swizzle(Nz::ShaderBuilder::AccessMember(Nz::ShaderBuilder::Identifier(uniform), std::vector<std::size_t>{ 0, 0 }, Nz::ShaderNodes::BasicType::Float3), Nz::ShaderNodes::SwizzleComponent::Third);
			auto assign = Nz::ShaderBuilder::Assign(Nz::ShaderBuilder::Identifier(output), access);

			shader.AddFunction("main", Nz::ShaderBuilder::ExprStatement(assign));

			SECTION("Generating GLSL")
			{
				ExpectingGLSL(shader, R"(
void main()
{
	result = ubo.s.field.z;
}
)");
			}
			SECTION("Generating Spir-V")
			{
				ExpectingSpirV(shader, R"(
OpFunction
OpLabel
OpAccessChain
OpLoad
OpCompositeExtract
OpStore
OpReturn
OpFunctionEnd)");
			}
		}
	}
}
