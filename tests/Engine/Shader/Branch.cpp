#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("branching", "[Shader]")
{
	WHEN("using a simple branch")
	{
		std::string_view nzslSource = R"(
struct inputStruct
{
	value: f32
}

external
{
	[set(0), binding(0)] data: uniform<inputStruct>
}

[entry(frag)]
fn main()
{
	let value: f32;
	if (data.value > 0.0)
		value = 1.0;
	else
		value = 0.0;
}
)";

		Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

		ExpectGLSL(*shader, R"(
void main()
{
	float value;
	if (data.value > (0.000000))
	{
		value = 1.000000;
	}
	else
	{
		value = 0.000000;
	}
	
}
)");

		ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let value: f32;
	if (data.value > (0.000000))
	{
		value = 1.000000;
	}
	else
	{
		value = 0.000000;
	}
	
}
)");

		ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpAccessChain
OpLoad
OpFOrdGreaterThanEqual
OpSelectionMerge
OpBranchConditional
OpLabel
OpStore
OpBranch
OpLabel
OpStore
OpBranch
OpLabel
OpReturn
OpFunctionEnd)");
	}

	WHEN("discarding in a branch")
	{
		std::string_view nzslSource = R"(
struct inputStruct
{
	value: f32
}

external
{
	[set(0), binding(0)] data: uniform<inputStruct>
}

[entry(frag)]
fn main()
{
	if (data.value > 0.0)
		discard;
}
)";

		Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

		ExpectGLSL(*shader, R"(
void main()
{
	if (data.value > (0.000000))
	{
		discard;
	}
	
}
)");

		ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	if (data.value > (0.000000))
	{
		discard;
	}
	
}
)");

		ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpAccessChain
OpLoad
OpFOrdGreaterThanEqual
OpSelectionMerge
OpBranchConditional
OpLabel
OpKill
OpLabel
OpReturn
OpFunctionEnd)");
	}
}
