#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <catch2/catch.hpp>
#include <cctype>

void ExpectingGLSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	Nz::GlslWriter writer;

	std::string output = writer.Generate(shader);
	std::size_t funcOffset = output.find("void main()");
	std::string_view subset = Nz::Trim(output).substr(funcOffset);
	expectedOutput = Nz::Trim(expectedOutput);

	REQUIRE(subset == expectedOutput);
}

void ExpectingSpirV(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	Nz::SpirvWriter writer;
	auto spirv = writer.Generate(shader);

	Nz::SpirvPrinter printer;

	Nz::SpirvPrinter::Settings settings;
	settings.printHeader = false;
	settings.printParameters = false;

	std::string output = printer.Print(spirv.data(), spirv.size(), settings);
	std::size_t funcOffset = output.find("OpFunction");
	std::string_view subset = Nz::Trim(output).substr(funcOffset);
	expectedOutput = Nz::Trim(expectedOutput);

	REQUIRE(subset == expectedOutput);
}

SCENARIO("Shader generation", "[Shader]")
{
	SECTION("Nested member loading")
	{
		std::vector<Nz::ShaderAst::StatementPtr> statements;

		Nz::ShaderAst::StructDescription innerStructDesc;
		{
			innerStructDesc.name = "innerStruct";
			auto& member = innerStructDesc.members.emplace_back();
			member.name = "field";
			member.type = Nz::ShaderAst::VectorType{ 3, Nz::ShaderAst::PrimitiveType::Float32 };
		}
		statements.push_back(Nz::ShaderBuilder::DeclareStruct(std::move(innerStructDesc)));

		Nz::ShaderAst::StructDescription outerStruct;
		{
			outerStruct.name = "outerStruct";
			auto& member = outerStruct.members.emplace_back();
			member.name = "s";
			member.type = Nz::ShaderAst::IdentifierType{ "innerStruct" };
		}
		statements.push_back(Nz::ShaderBuilder::DeclareStruct(std::move(outerStruct)));

		auto external = std::make_unique<Nz::ShaderAst::DeclareExternalStatement>();

		auto& externalVar = external->externalVars.emplace_back();
		externalVar.bindingIndex = 0;
		externalVar.name = "ubo";
		externalVar.type = Nz::ShaderAst::UniformType{ Nz::ShaderAst::IdentifierType{ "outerStruct" } };

		statements.push_back(std::move(external));

		SECTION("Nested AccessMember")
		{
			auto ubo = Nz::ShaderBuilder::Identifier("ubo");
			auto firstAccess = Nz::ShaderBuilder::AccessMember(std::move(ubo), { "s" });
			auto secondAccess = Nz::ShaderBuilder::AccessMember(std::move(firstAccess), { "field" });

			auto swizzle = Nz::ShaderBuilder::Swizzle(std::move(secondAccess), { 2u });
			auto varDecl = Nz::ShaderBuilder::DeclareVariable("result", Nz::ShaderAst::PrimitiveType::Float32, std::move(swizzle));

			statements.push_back(Nz::ShaderBuilder::DeclareFunction("main", std::move(varDecl)));

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderBuilder::MultiStatement(std::move(statements));

			SECTION("Generating GLSL")
			{
				ExpectingGLSL(*shader, R"(
void main()
{
	float result = ubo.s.field.z;
}
)");
			}
			SECTION("Generating Spir-V")
			{
				ExpectingSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
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
			auto ubo = Nz::ShaderBuilder::Identifier("ubo");
			auto access = Nz::ShaderBuilder::AccessMember(std::move(ubo), { "s", "field" });

			auto swizzle = Nz::ShaderBuilder::Swizzle(std::move(access), { 2u });
			auto varDecl = Nz::ShaderBuilder::DeclareVariable("result", Nz::ShaderAst::PrimitiveType::Float32, std::move(swizzle));

			statements.push_back(Nz::ShaderBuilder::DeclareFunction("main", std::move(varDecl)));

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderBuilder::MultiStatement(std::move(statements));

			SECTION("Generating GLSL")
			{
				ExpectingGLSL(*shader, R"(
void main()
{
	float result = ubo.s.field.z;
}
)");
			}
			SECTION("Generating Spir-V")
			{
				ExpectingSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
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
