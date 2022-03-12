#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstConstantPropagationVisitor.hpp>
#include <Nazara/Shader/Ast/EliminateUnusedPassVisitor.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

template<typename T, typename U>
std::unique_ptr<T> static_unique_pointer_cast(std::unique_ptr<U>&& ptr)
{
	return std::unique_ptr<T>(Nz::SafeCast<T*>(ptr.release()));
}

void PropagateConstantAndExpect(std::string_view sourceCode, std::string_view expectedOptimizedResult)
{
	Nz::ShaderAst::ModulePtr shaderModule;
	REQUIRE_NOTHROW(shaderModule = Nz::ShaderLang::Parse(sourceCode));
	shaderModule = SanitizeModule(*shaderModule);
	REQUIRE_NOTHROW(shaderModule = Nz::ShaderAst::PropagateConstants(*shaderModule));

	ExpectNZSL(*shaderModule, expectedOptimizedResult);
}

void EliminateUnusedAndExpect(std::string_view sourceCode, std::string_view expectedOptimizedResult)
{
	Nz::ShaderAst::DependencyCheckerVisitor::Config depConfig;
	depConfig.usedShaderStages = Nz::ShaderStageType_All;

	Nz::ShaderAst::ModulePtr shaderModule;
	REQUIRE_NOTHROW(shaderModule = Nz::ShaderLang::Parse(sourceCode));
	shaderModule = SanitizeModule(*shaderModule);
	REQUIRE_NOTHROW(shaderModule = Nz::ShaderAst::EliminateUnusedPass(*shaderModule, depConfig));

	ExpectNZSL(*shaderModule, expectedOptimizedResult);
}

TEST_CASE("optimizations", "[Shader]")
{
	WHEN("propagating constants")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let output = 8.0 * (7.0 + 5.0) * 2.0 / 4.0 - 6.0;
}
)", R"(
[entry(frag)]
fn main()
{
	let output: f32 = 42.000000;
}
)");
	}

	WHEN("propagating vector constants")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let output = vec4[f32](8.0, 2.0, -7.0, 0.0) * (7.0 + 5.0) * 2.0 / 4.0;
}
)", R"(
[entry(frag)]
fn main()
{
	let output: vec4[f32] = vec4[f32](48.000000, 12.000000, -42.000000, 0.000000);
)");
	}

	WHEN("eliminating simple branch")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	if (5 + 3 < 2)
		discard;
}
)", R"(
[entry(frag)]
fn main()
{
	
}
)");
	}

	WHEN("eliminating multiple branches")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let output = 0.0;
	if (5 <= 3)
		output = 5.0;
	else if (4 <= 3)
		output = 4.0;
	else if (3 <= 3)
		output = 3.0;
	else if (2 <= 3)
		output = 2.0;
	else if (1 <= 3)
		output = 1.0;
	else
		output = 0.0;
}
)", R"(
[entry(frag)]
fn main()
{
	let output: f32 = 0.000000;
	output = 3.000000;
}
)");
	}


	WHEN("eliminating multiple split branches")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let output = 0.0;
	if (5 <= 3)
		output = 5.0;
	else
	{
		if (4 <= 3)
			output = 4.0;
		else
		{
			if (3 <= 3)
				output = 3.0;
			else
			{
				if (2 <= 3)
					output = 2.0;
				else
				{
					if (1 <= 3)
						output = 1.0;
					else
						output = 0.0;
				}
			}
		}
	}
}
)", R"(
[entry(frag)]
fn main()
{
	let output: f32 = 0.000000;
	output = 3.000000;
}
)");
	}

	WHEN("optimizing out scalar swizzle")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let value = vec3[f32](3.0, 0.0, 1.0).z;
}
)", R"(
[entry(frag)]
fn main()
{
	let value: f32 = 1.000000;
}
)");
	}

	WHEN("optimizing out scalar swizzle to vector")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let value = (42.0).xxxx;
}
)", R"(
[entry(frag)]
fn main()
{
	let value: vec4[f32] = vec4[f32](42.000000, 42.000000, 42.000000, 42.000000);
}
)");
	}

	WHEN("optimizing out vector swizzle")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let value = vec4[f32](3.0, 0.0, 1.0, 2.0).yzwx;
}
)", R"(
[entry(frag)]
fn main()
{
	let value: vec4[f32] = vec4[f32](0.000000, 1.000000, 2.000000, 3.000000);
}
)");
	}

	WHEN("optimizing out vector swizzle with repetition")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let value = vec4[f32](3.0, 0.0, 1.0, 2.0).zzxx;
}
)", R"(
[entry(frag)]
fn main()
{
	let value: vec4[f32] = vec4[f32](1.000000, 1.000000, 3.000000, 3.000000);
}
)");
	}

	WHEN("optimizing out complex swizzle")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let value = vec4[f32](0.0, 1.0, 2.0, 3.0).xyz.yz.y.x.xxxx;
}
)", R"(
[entry(frag)]
fn main()
{
	let value: vec4[f32] = vec4[f32](2.000000, 2.000000, 2.000000, 2.000000);
}
)");
	}

	WHEN("optimizing out complex swizzle on unknown value")
	{
		PropagateConstantAndExpect(R"(
[nzsl_version("1.0")]
module;

struct inputStruct
{
	value: vec4[f32]
}

external
{
	[set(0), binding(0)] data: uniform[inputStruct]
}

[entry(frag)]
fn main()
{
	let value = data.value.xyz.yz.y.x.xxxx;
}
)", R"(
[entry(frag)]
fn main()
{
	let value: vec4[f32] = data.value.zzzz;
}
)");
	}

	WHEN("eliminating unused code")
	{
		EliminateUnusedAndExpect(R"(
[nzsl_version("1.0")]
module;

struct inputStruct
{
	value: vec4[f32]
}

struct notUsed
{
	value: vec4[f32]
}

external
{
	[set(0), binding(0)] unusedData: uniform[notUsed],
	[set(0), binding(1)] data: uniform[inputStruct]
}

fn unusedFunction() -> vec4[f32]
{
	return unusedData.value;
}

struct Output
{
	value: vec4[f32]
}

[entry(frag)]
fn main() -> Output
{
	let unusedvalue = unusedFunction();

	let output: Output;
	output.value = data.value;
	return output;
})", R"(
[nzsl_version("1.0")]
module;

struct inputStruct
{
	value: vec4[f32]
}

external
{
	[set(0), binding(1)] data: uniform[inputStruct]
}

struct Output
{
	value: vec4[f32]
}

[entry(frag)]
fn main() -> Output
{
	let output: Output;
	output.value = data.value;
	return output;
})");
	}
}
