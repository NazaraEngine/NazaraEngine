#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <Nazara/Shader/Ast/AstCompare.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

void ParseSerializeUnserialize(std::string_view sourceCode, bool sanitize)
{
	Nz::ShaderAst::ModulePtr shaderModule;
	REQUIRE_NOTHROW(shaderModule = Nz::ShaderLang::Parse(sourceCode));

	if (sanitize)
		REQUIRE_NOTHROW(shaderModule = Nz::ShaderAst::Sanitize(*shaderModule));

	Nz::ByteArray serializedModule;
	REQUIRE_NOTHROW(serializedModule = Nz::ShaderAst::SerializeShader(*shaderModule));

	Nz::ByteStream byteStream(&serializedModule);
	Nz::ShaderAst::ModulePtr unserializedShader;
	REQUIRE_NOTHROW(unserializedShader = Nz::ShaderAst::UnserializeShader(byteStream));

	CHECK(Nz::ShaderAst::Compare(*shaderModule, *unserializedShader));
}

void ParseSerializeUnserialize(std::string_view sourceCode)
{
	ParseSerializeUnserialize(sourceCode, false);
	ParseSerializeUnserialize(sourceCode, true);
}

TEST_CASE("serialization", "[Shader]")
{
	WHEN("serializing and unserializing a simple shader")
	{
		ParseSerializeUnserialize(R"(
[nzsl_version("1.0")]
module;

struct Data
{
	value: f32
}

struct Output
{
	[location(0)] value: f32
}

external
{
	[set(0), binding(0)] data: uniform[Data]
}

[entry(frag)]
fn main() -> Output
{
	let output: Output;
	output.value = data.value;

	return output;
}
)");
	}

	WHEN("serializing and unserializing branches")
	{
		ParseSerializeUnserialize(R"(
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
)");
	}

	WHEN("serializing and unserializing consts")
	{
		ParseSerializeUnserialize(R"(
[nzsl_version("1.0")]
module;

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
	[set(0), binding(0)] data: uniform[inputStruct]
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
)");
	}

	WHEN("serializing and unserializing loops")
	{
		ParseSerializeUnserialize(R"(
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
	let value = 0.0;
	let i = 0;
	while (i < 10)
	{
		value += 0.1;
		i += 1;
	}

	let x = 0;
	for v in 0 -> 10
	{
		x += v;
	}

	let x = 0.0;
	for v in data.value
	{
		x += v;
	}
}
)");
	}

	WHEN("serializing and unserializing swizzles")
	{
		ParseSerializeUnserialize(R"(
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let vec = vec4[f32](0.0, 1.0, 2.0, 3.0);
	vec.wyxz.bra.ts.x = 0.0;
	vec.zyxw.ar.xy.yx = vec2[f32](1.0, 0.0);
}
)");
	}
}
