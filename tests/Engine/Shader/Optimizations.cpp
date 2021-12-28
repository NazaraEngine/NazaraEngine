#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstOptimizer.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

void ExpectOptimization(std::string_view sourceCode, std::string_view expectedOptimizedResult)
{
	Nz::ShaderAst::StatementPtr shader;
	REQUIRE_NOTHROW(shader = Nz::ShaderLang::Parse(sourceCode));
	REQUIRE_NOTHROW(shader = Nz::ShaderAst::Sanitize(*shader));
	REQUIRE_NOTHROW(shader = Nz::ShaderAst::Optimize(*shader));

	ExpectNZSL(*shader, expectedOptimizedResult);
}

TEST_CASE("optimizations", "[Shader]")
{
	WHEN("propaging constants")
	{
		ExpectOptimization(R"(
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
)");
	}

	WHEN("propaging vector constants")
	{
		ExpectOptimization(R"(
[entry(frag)]
fn main()
{
	let output = vec4<f32>(8.0, 2.0, -7.0, 0.0) * (7.0 + 5.0) * 2.0 / 4.0;
}
)", R"(
[entry(frag)]
fn main()
{
	let output: vec4<f32> = vec4<f32>(48.000000, 12.000000, -42.000000, 0.000000);
)");
	}

	WHEN("eliminating simple branch")
	{
		ExpectOptimization(R"(
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
		ExpectOptimization(R"(
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


	WHEN("eliminating multiple splitted branches")
	{
		ExpectOptimization(R"(
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
}
