#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("loops", "[Shader]")
{
	WHEN("using a while")
	{
		std::string_view nzslSource = R"(
[nzsl_version("1.0")]
module;

struct inputStruct
{
	value: f32
}

external
{
	[set(0), binding(0)] data: uniform[inputStruct]
}

[entry(frag)]
fn main()
{
	let value = 0.0;
	let i = 0;
	while (i < 10)
	{
		value += 0.1;
		i += 1;
	}
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);

		ExpectGLSL(*shaderModule, R"(
void main()
{
	float value = 0.000000;
	int i = 0;
	while (i < (10))
	{
		value += 0.100000;
		i += 1;
	}
	
}
)");

		ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let value: f32 = 0.000000;
	let i: i32 = 0;
	while (i < (10))
	{
		value += 0.100000;
		i += 1;
	}
	
}
)");

		ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpStore
OpStore
OpBranch
OpLabel
OpLoad
OpSLessThan
OpLoopMerge
OpBranchConditional
OpLabel
OpLoad
OpFAdd
OpStore
OpLoad
OpIAdd
OpStore
OpBranch
OpLabel
OpReturn
OpFunctionEnd)");
	}
	
	WHEN("using a for range")
	{
		std::string_view nzslSource = R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let x = 0;
	for v in 0 -> 10
	{
		x += v;
	}
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);


		ExpectGLSL(*shaderModule, R"(
void main()
{
	int x = 0;
	int v = 0;
	int to = 10;
	while (v < to)
	{
		x += v;
		v += 1;
	}
	
}
)");

		ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let x: i32 = 0;
	for v in 0 -> 10
	{
		x += v;
	}
	
}
)");

		ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpVariable
OpStore
OpStore
OpStore
OpBranch
OpLabel
OpLoad
OpLoad
OpSLessThan
OpLoopMerge
OpBranchConditional
OpLabel
OpLoad
OpLoad
OpIAdd
OpStore
OpLoad
OpIAdd
OpStore
OpBranch
OpLabel
OpReturn
OpFunctionEnd)");
	}

	WHEN("using a for range with step")
	{
		std::string_view nzslSource = R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let x = 0;
	for v in 0 -> 10 : 2
	{
		x += v;
	}
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);


		ExpectGLSL(*shaderModule, R"(
void main()
{
	int x = 0;
	int v = 0;
	int to = 10;
	int step = 2;
	while (v < to)
	{
		x += v;
		v += step;
	}
	
}
)");

		ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let x: i32 = 0;
	for v in 0 -> 10 : 2
	{
		x += v;
	}
	
}
)");

		ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpVariable
OpVariable
OpStore
OpStore
OpStore
OpStore
OpBranch
OpLabel
OpLoad
OpLoad
OpSLessThan
OpLoopMerge
OpBranchConditional
OpLabel
OpLoad
OpLoad
OpIAdd
OpStore
OpLoad
OpLoad
OpIAdd
OpStore
OpBranch
OpLabel
OpReturn
OpFunctionEnd)");
	}

	WHEN("using a for-each")
	{
		std::string_view nzslSource = R"(
[nzsl_version("1.0")]
module;

struct inputStruct
{
	value: array[f32, 10]
}

external
{
	[set(0), binding(0)] data: uniform[inputStruct]
}

[entry(frag)]
fn main()
{
	let x = 0.0;
	for v in data.value
	{
		x += v;
	}
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);


		ExpectGLSL(*shaderModule, R"(
void main()
{
	float x = 0.000000;
	uint i = 0u;
	while (i < (10u))
	{
		float v = data.value[i];
		x += v;
		i += 1u;
	}
	
}
)");

		ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let x: f32 = 0.000000;
	for v in data.value
	{
		x += v;
	}
	
}
)");

		ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpVariable
OpStore
OpStore
OpBranch
OpLabel
OpLoad
OpULessThan
OpLoopMerge
OpBranchConditional
OpLabel
OpLoad
OpAccessChain
OpLoad
OpStore
OpLoad
OpLoad
OpFAdd
OpStore
OpLoad
OpIAdd
OpStore
OpBranch
OpLabel
OpReturn
OpFunctionEnd)");
	}
}
