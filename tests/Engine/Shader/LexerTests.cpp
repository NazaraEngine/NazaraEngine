#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("lexer", "[Shader]")
{
	SECTION("Simple code")
	{
		std::string_view nzslSource = R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let vec = vec4[f32](0.0, 1.0, 2.0, 3.0);
	let i = 42;
	let value = vec.xyz;
}
)";

		std::vector<Nz::ShaderLang::Token> tokens = Nz::ShaderLang::Tokenize(nzslSource);
		CHECK(Nz::ShaderLang::ToString(tokens) == R"(OpenSquareBracket Identifier(nzsl_version) OpenParenthesis StringValue("1.0") ClosingParenthesis ClosingSquareBracket
Module Semicolon
OpenSquareBracket Identifier(entry) OpenParenthesis Identifier(frag) ClosingParenthesis ClosingSquareBracket
FunctionDeclaration Identifier(main) OpenParenthesis ClosingParenthesis
OpenCurlyBracket
Let Identifier(vec) Assign Identifier(vec4) OpenSquareBracket Identifier(f32) ClosingSquareBracket OpenParenthesis FloatingPointValue(0) Comma FloatingPointValue(1) Comma FloatingPointValue(2) Comma FloatingPointValue(3) ClosingParenthesis Semicolon
Let Identifier(i) Assign IntegerValue(42) Semicolon
Let Identifier(value) Assign Identifier(vec) Dot Identifier(xyz) Semicolon
ClosingCurlyBracket
EndOfStream)");
	}
}
