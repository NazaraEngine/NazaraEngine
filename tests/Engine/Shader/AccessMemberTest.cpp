#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <catch2/catch.hpp>
#include <cctype>

SCENARIO("Shader generation", "[Shader]")
{
	SECTION("Nested member loading")
	{
		std::string_view nzslSource = R"(
struct innerStruct
{
	field: vec3<f32>
}

struct outerStruct
{
	s: innerStruct
}

external
{
	[set(0), binding(0)] ubo: uniform<outerStruct>
}
)";

		Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);
		REQUIRE(shader->GetType() == Nz::ShaderAst::NodeType::MultiStatement);
		Nz::ShaderAst::MultiStatement& multiStatement = static_cast<Nz::ShaderAst::MultiStatement&>(*shader);

		SECTION("Nested AccessMember")
		{
			auto ubo = Nz::ShaderBuilder::Identifier("ubo");
			auto firstAccess = Nz::ShaderBuilder::AccessMember(std::move(ubo), { "s" });
			auto secondAccess = Nz::ShaderBuilder::AccessMember(std::move(firstAccess), { "field" });

			auto swizzle = Nz::ShaderBuilder::Swizzle(std::move(secondAccess), { 2u });
			auto varDecl = Nz::ShaderBuilder::DeclareVariable("result", Nz::ShaderAst::PrimitiveType::Float32, std::move(swizzle));

			multiStatement.statements.push_back(Nz::ShaderBuilder::DeclareFunction(Nz::ShaderStageType::Vertex, "main", std::move(varDecl)));

			ExpectingGLSL(*shader, R"(
void main()
{
	float result = ubo.s.field.z;
}
)");

			ExpectingNZSL(*shader, R"(
[entry(vert)]
fn main()
{
	let result: f32 = ubo.s.field.z;
}
)");

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

		SECTION("AccessMember with multiples fields")
		{
			auto ubo = Nz::ShaderBuilder::Identifier("ubo");
			auto access = Nz::ShaderBuilder::AccessMember(std::move(ubo), { "s", "field" });

			auto swizzle = Nz::ShaderBuilder::Swizzle(std::move(access), { 2u });
			auto varDecl = Nz::ShaderBuilder::DeclareVariable("result", Nz::ShaderAst::PrimitiveType::Float32, std::move(swizzle));

			multiStatement.statements.push_back(Nz::ShaderBuilder::DeclareFunction(Nz::ShaderStageType::Vertex, "main", std::move(varDecl)));

			ExpectingGLSL(*shader, R"(
void main()
{
	float result = ubo.s.field.z;
}
)");

			ExpectingNZSL(*shader, R"(
[entry(vert)]
fn main()
{
	let result: f32 = ubo.s.field.z;
}
)");

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
