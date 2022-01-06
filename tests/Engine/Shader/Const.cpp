#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstOptimizer.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

void ExpectOutput(Nz::ShaderAst::Statement& shader, const Nz::ShaderAst::SanitizeVisitor::Options& options, std::string_view expectedOptimizedResult)
{
	Nz::ShaderAst::StatementPtr sanitizedShader;
	REQUIRE_NOTHROW(sanitizedShader = Nz::ShaderAst::Sanitize(shader, options));

	ExpectNZSL(*sanitizedShader, expectedOptimizedResult);
}

TEST_CASE("const", "[Shader]")
{
	WHEN("using const if")
	{
		std::string_view sourceCode = R"(
option UseInt: bool = false;

[cond(UseInt)]
struct inputStruct
{
	value: i32
}

[cond(!UseInt)]
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

	const if (UseInt)
	{
		value = f32(data.value);
	}
	else
	{
		value = data.value;
	}
}
)";

		Nz::ShaderAst::StatementPtr shader;
		REQUIRE_NOTHROW(shader = Nz::ShaderLang::Parse(sourceCode));

		Nz::ShaderAst::SanitizeVisitor::Options options;

		WHEN("Enabling option")
		{
			options.optionValues[0] = true;

			ExpectOutput(*shader, options, R"(
struct inputStruct
{
	value: i32
}

external
{
	[set(0), binding(0)] data: uniform<inputStruct>
}

[entry(frag)]
fn main()
{
	let value: f32;
	value = f32(data.value);
}
)");
		}

		WHEN("Disabling option")
		{
			options.optionValues[0] = false;

			ExpectOutput(*shader, options, R"(
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
	value = data.value;
}
)");
		}
	}
	
	WHEN("using [unroll] attribute on numerical for")
	{
		std::string_view sourceCode = R"(
const LightCount = 3;

[layout(std140)]
struct Light
{
	color: vec4<f32>
}

[layout(std140)]
struct LightData
{
	lights: [Light; LightCount]
}

external
{
	[set(0), binding(0)] data: uniform<LightData>
}

[entry(frag)]
fn main()
{
	let color = (0.0).xxxx;

	[unroll]
	for i in 0 -> 10 : 2
	{
		color += data.lights[i].color;
	}
}
)";

		Nz::ShaderAst::StatementPtr shader;
		REQUIRE_NOTHROW(shader = Nz::ShaderLang::Parse(sourceCode));

		ExpectOutput(*shader, {}, R"(
[entry(frag)]
fn main()
{
	let color: vec4<f32> = (0.000000).xxxx;
	let i: i32 = 0;
	color += data.lights[i].color;
	let i: i32 = 2;
	color += data.lights[i].color;
	let i: i32 = 4;
	color += data.lights[i].color;
	let i: i32 = 6;
	color += data.lights[i].color;
	let i: i32 = 8;
	color += data.lights[i].color;
}
)");
	}

	WHEN("using [unroll] attribute on for-each")
	{
		std::string_view sourceCode = R"(
const LightCount = 3;

[layout(std140)]
struct Light
{
	color: vec4<f32>
}

[layout(std140)]
struct LightData
{
	lights: [Light; LightCount]
}

external
{
	[set(0), binding(0)] data: uniform<LightData>
}

[entry(frag)]
fn main()
{
	let color = (0.0).xxxx;

	[unroll]
	for light in data.lights
	{
		color += light.color;
	}
}
)";

		Nz::ShaderAst::StatementPtr shader;
		REQUIRE_NOTHROW(shader = Nz::ShaderLang::Parse(sourceCode));

		ExpectOutput(*shader, {}, R"(
[entry(frag)]
fn main()
{
	let color: vec4<f32> = (0.000000).xxxx;
	let light: Light = data.lights[0];
	color += light.color;
	let light: Light = data.lights[1];
	color += light.color;
	let light: Light = data.lights[2];
	color += light.color;
}
)");
	}
}
