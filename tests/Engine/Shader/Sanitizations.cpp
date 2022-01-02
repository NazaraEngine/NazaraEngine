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
	[set(0), binding(0)] data: uniform<inputStruct>
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
	value: [f32; 10]
}

external
{
	[set(0), binding(0)] data: uniform<inputStruct>
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
}
