#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("swizzle", "[Shader]")
{
	SECTION("Simple swizzle")
	{
		WHEN("reading")
		{
			std::string_view nzslSource = R"(
[entry(frag)]
fn main()
{
	let vec = vec4<f32>(0.0, 1.0, 2.0, 3.0);
	let value = vec.xyz;
}
)";

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

			ExpectGLSL(*shader, R"(
void main()
{
	vec4 vec = vec4(0.000000, 1.000000, 2.000000, 3.000000);
	vec3 value = vec.xyz;
}
)");

			ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let vec: vec4<f32> = vec4<f32>(0.000000, 1.000000, 2.000000, 3.000000);
	let value: vec3<f32> = vec.xyz;
}
)");

			ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpCompositeConstruct
OpStore
OpLoad
OpVectorShuffle
OpStore
OpReturn
OpFunctionEnd)");
		}

		WHEN("writing")
		{
			std::string_view nzslSource = R"(
[entry(frag)]
fn main()
{
	let vec = vec4<f32>(0.0, 0.0, 0.0, 0.0);
	vec.yzw = vec3<f32>(1.0, 2.0, 3.0);
}
)";

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

			ExpectGLSL(*shader, R"(
void main()
{
	vec4 vec = vec4(0.000000, 0.000000, 0.000000, 0.000000);
	vec.yzw = vec3(1.000000, 2.000000, 3.000000);
}
)");

			ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let vec: vec4<f32> = vec4<f32>(0.000000, 0.000000, 0.000000, 0.000000);
	vec.yzw = vec3<f32>(1.000000, 2.000000, 3.000000);
}
)");

			ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpCompositeConstruct
OpStore
OpCompositeConstruct
OpLoad
OpVectorShuffle
OpStore
OpReturn
OpFunctionEnd)");
		}
	}
	
	SECTION("Scalar swizzle")
	{
		GIVEN("a variable")
		{
			std::string_view nzslSource = R"(
[entry(frag)]
fn main()
{
	let value = 42.0;
	let vec = value.xxx;
}
)";

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

			ExpectGLSL(*shader, R"(
void main()
{
	float value = 42.000000;
	vec3 vec = vec3(value, value, value);
}
)");

			ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let value: f32 = 42.000000;
	let vec: vec3<f32> = value.xxx;
}
)");

			ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpStore
OpLoad
OpCompositeConstruct
OpStore
OpReturn
OpFunctionEnd)");
		}

		GIVEN("a function value")
		{
			std::string_view nzslSource = R"(
[entry(frag)]
fn main()
{
	let vec = max(2.0, 1.0).xxx;
}
)";

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

			ExpectGLSL(*shader, R"(
void main()
{
	float cachedResult = max(2.000000, 1.000000);
	vec3 vec = vec3(cachedResult, cachedResult, cachedResult);
}
)");

			ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let vec: vec3<f32> = (max(2.000000, 1.000000)).xxx;
}
)");

			ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpExtInst
OpCompositeConstruct
OpStore
OpReturn
OpFunctionEnd)");
		}
	}

	SECTION("Complex swizzle")
	{
		WHEN("reading")
		{
			std::string_view nzslSource = R"(
[entry(frag)]
fn main()
{
	let vec = vec4<f32>(0.0, 1.0, 2.0, 3.0);
	let value = vec.xyz.yz.y.x.xxxx;
}
)";

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

			ExpectGLSL(*shader, R"(
void main()
{
	vec4 vec = vec4(0.000000, 1.000000, 2.000000, 3.000000);
	vec4 value = vec4(vec.xyz.yz.y, vec.xyz.yz.y, vec.xyz.yz.y, vec.xyz.yz.y);
}
)");

			ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let vec: vec4<f32> = vec4<f32>(0.000000, 1.000000, 2.000000, 3.000000);
	let value: vec4<f32> = vec.xyz.yz.y.x.xxxx;
}
)");

			ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpCompositeConstruct
OpStore
OpLoad
OpVectorShuffle
OpVectorShuffle
OpCompositeExtract
OpCompositeConstruct
OpStore
OpReturn
OpFunctionEnd)");
		}

		WHEN("writing")
		{
			std::string_view nzslSource = R"(
[entry(frag)]
fn main()
{
	let vec = vec4<f32>(0.0, 1.0, 2.0, 3.0);
	vec.wyxz.bra.ts.x = 0.0;
	vec.zyxw.ar.xy.yx = vec2<f32>(1.0, 0.0);
}
)";

			Nz::ShaderAst::StatementPtr shader = Nz::ShaderLang::Parse(nzslSource);

			ExpectGLSL(*shader, R"(
void main()
{
	vec4 vec = vec4(0.000000, 1.000000, 2.000000, 3.000000);
	vec.wyxz.zxw.yx.x = 0.000000;
	vec.zyxw.wx.xy.yx = vec2(1.000000, 0.000000);
}
)");

			ExpectNZSL(*shader, R"(
[entry(frag)]
fn main()
{
	let vec: vec4<f32> = vec4<f32>(0.000000, 1.000000, 2.000000, 3.000000);
	vec.wyxz.zxw.yx.x = 0.000000;
	vec.zyxw.wx.xy.yx = vec2<f32>(1.000000, 0.000000);
}
)");

			ExpectSpirV(*shader, R"(
OpFunction
OpLabel
OpVariable
OpCompositeConstruct
OpStore
OpAccessChain
OpStore
OpCompositeConstruct
OpLoad
OpVectorShuffle
OpStore
OpReturn
OpFunctionEnd)");
		}
	}
}
