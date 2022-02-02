#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("sanitizing", "[Shader]")
{
	WHEN("splitting branches")
	{
		std::string_view nzslSource = R"(
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
	let value: f32;
	if (data.value > 3.0)
		value = 3.0;
	else if (data.value > 2.0)
		value = 2.0;
	else if (data.value > 1.0)
		value = 1.0;
	else
		value = 0.0;
}
)";

		Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

		Nz::ShaderAst::SanitizeVisitor::Options options;
		options.splitMultipleBranches = true;

		REQUIRE_NOTHROW(shader = Nz::ShaderAst::Sanitize(*shader, options));

		ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let value: f32;
	if (data.value > (3.000000))
	{
		value = 3.000000;
	}
	else
	{
		if (data.value > (2.000000))
		{
			value = 2.000000;
		}
		else
		{
			if (data.value > (1.000000))
			{
				value = 1.000000;
			}
			else
			{
				value = 0.000000;
			}
			
		}
		
	}
	
}
)");

	}

	WHEN("reducing for-each to while")
	{
		std::string_view nzslSource = R"(
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

		Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

		Nz::ShaderAst::SanitizeVisitor::Options options;
		options.reduceLoopsToWhile = true;

		REQUIRE_NOTHROW(shader = Nz::ShaderAst::Sanitize(*shader, options));

		ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let x: f32 = 0.000000;
	let i: u32 = 0;
	while (i < (10))
	{
		let v: f32 = data.value[i];
		x += v;
		i += 1;
	}
	
}
)");

	}

	WHEN("removing matrix casts")
	{
		std::string_view nzslSource = R"(
fn testMat2ToMat2(input: mat2[f32]) -> mat2[f32]
{
	return mat2[f32](input);
}

fn testMat2ToMat3(input: mat2[f32]) -> mat3[f32]
{
	return mat3[f32](input);
}

fn testMat2ToMat4(input: mat2[f32]) -> mat4[f32]
{
	return mat4[f32](input);
}

fn testMat3ToMat2(input: mat3[f32]) -> mat2[f32]
{
	return mat2[f32](input);
}

fn testMat3ToMat3(input: mat3[f32]) -> mat3[f32]
{
	return mat3[f32](input);
}

fn testMat3ToMat4(input: mat3[f32]) -> mat4[f32]
{
	return mat4[f32](input);
}

fn testMat4ToMat2(input: mat4[f32]) -> mat2[f32]
{
	return mat2[f32](input);
}

fn testMat4ToMat3(input: mat4[f32]) -> mat3[f32]
{
	return mat3[f32](input);
}

fn testMat4ToMat4(input: mat4[f32]) -> mat4[f32]
{
	return mat4[f32](input);
}
)";

		Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

		Nz::ShaderAst::SanitizeVisitor::Options options;
		options.removeMatrixCast = true;

		REQUIRE_NOTHROW(shader = Nz::ShaderAst::Sanitize(*shader, options));

		ExpectNZSL(*shader, R"(
fn testMat2ToMat2(input: mat2[f32]) -> mat2[f32]
{
	return input;
}

fn testMat2ToMat3(input: mat2[f32]) -> mat3[f32]
{
	let temp: mat3[f32];
	temp[0] = vec3[f32](input[0], 0.000000);
	temp[1] = vec3[f32](input[1], 0.000000);
	temp[2] = vec3[f32](input[2], 1.000000);
	return temp;
}

fn testMat2ToMat4(input: mat2[f32]) -> mat4[f32]
{
	let temp: mat4[f32];
	temp[0] = vec4[f32](input[0], 0.000000, 0.000000);
	temp[1] = vec4[f32](input[1], 0.000000, 0.000000);
	temp[2] = vec4[f32](input[2], 1.000000, 0.000000);
	temp[3] = vec4[f32](input[3], 0.000000, 1.000000);
	return temp;
}

fn testMat3ToMat2(input: mat3[f32]) -> mat2[f32]
{
	let temp: mat2[f32];
	temp[0] = input[0].xy;
	temp[1] = input[1].xy;
	return temp;
}

fn testMat3ToMat3(input: mat3[f32]) -> mat3[f32]
{
	return input;
}

fn testMat3ToMat4(input: mat3[f32]) -> mat4[f32]
{
	let temp: mat4[f32];
	temp[0] = vec4[f32](input[0], 0.000000);
	temp[1] = vec4[f32](input[1], 0.000000);
	temp[2] = vec4[f32](input[2], 0.000000);
	temp[3] = vec4[f32](input[3], 1.000000);
	return temp;
}

fn testMat4ToMat2(input: mat4[f32]) -> mat2[f32]
{
	let temp: mat2[f32];
	temp[0] = input[0].xy;
	temp[1] = input[1].xy;
	return temp;
}

fn testMat4ToMat3(input: mat4[f32]) -> mat3[f32]
{
	let temp: mat3[f32];
	temp[0] = input[0].xyz;
	temp[1] = input[1].xyz;
	temp[2] = input[2].xyz;
	return temp;
}

fn testMat4ToMat4(input: mat4[f32]) -> mat4[f32]
{
	return input;
}
)");

	}
}
