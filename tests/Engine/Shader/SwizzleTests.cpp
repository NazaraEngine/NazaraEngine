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
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let vec = vec4[f32](0.0, 1.0, 2.0, 3.0);
	let value = vec.xyz;
}
)";

			Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
			shaderModule = SanitizeModule(*shaderModule);

			ExpectGLSL(*shaderModule, R"(
void main()
{
	vec4 vec = vec4(0.000000, 1.000000, 2.000000, 3.000000);
	vec3 value = vec.xyz;
}
)");

			ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let vec: vec4[f32] = vec4[f32](0.000000, 1.000000, 2.000000, 3.000000);
	let value: vec3[f32] = vec.xyz;
}
)");

			ExpectSPIRV(*shaderModule, R"(
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
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let vec = vec4[f32](0.0, 0.0, 0.0, 0.0);
	vec.yzw = vec3[f32](1.0, 2.0, 3.0);
}
)";

			Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
			shaderModule = SanitizeModule(*shaderModule);

			ExpectGLSL(*shaderModule, R"(
void main()
{
	vec4 vec = vec4(0.000000, 0.000000, 0.000000, 0.000000);
	vec.yzw = vec3(1.000000, 2.000000, 3.000000);
}
)");

			ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let vec: vec4[f32] = vec4[f32](0.000000, 0.000000, 0.000000, 0.000000);
	vec.yzw = vec3[f32](1.000000, 2.000000, 3.000000);
}
)");

			ExpectSPIRV(*shaderModule, R"(
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
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let value = 42.0;
	let vec = value.xxx;
}
)";

			Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
			shaderModule = SanitizeModule(*shaderModule);

			ExpectGLSL(*shaderModule, R"(
void main()
{
	float value = 42.000000;
	vec3 vec = vec3(value, value, value);
}
)");

			ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let value: f32 = 42.000000;
	let vec: vec3[f32] = value.xxx;
}
)");

			ExpectSPIRV(*shaderModule, R"(
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
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let v = max(2.0, 1.0).xxx;
	let v2 = min(2.0, 1.0).xxx;
}
)";

			Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
			shaderModule = SanitizeModule(*shaderModule);

			ExpectGLSL(*shaderModule, R"(
void main()
{
	float cachedResult = max(2.000000, 1.000000);
	vec3 v = vec3(cachedResult, cachedResult, cachedResult);
	float cachedResult_2 = min(2.000000, 1.000000);
	vec3 v2 = vec3(cachedResult_2, cachedResult_2, cachedResult_2);
}
)");

			ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let v: vec3[f32] = (max(2.000000, 1.000000)).xxx;
	let v2: vec3[f32] = (min(2.000000, 1.000000)).xxx;
}
)");

			ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpExtInst
OpCompositeConstruct
OpStore
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
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let vec = vec4[f32](0.0, 1.0, 2.0, 3.0);
	let value = vec.xyz.yz.y.x.xxxx;
}
)";

			Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
			shaderModule = SanitizeModule(*shaderModule);

			ExpectGLSL(*shaderModule, R"(
void main()
{
	vec4 vec = vec4(0.000000, 1.000000, 2.000000, 3.000000);
	vec4 value = vec4(vec.xyz.yz.y, vec.xyz.yz.y, vec.xyz.yz.y, vec.xyz.yz.y);
}
)");

			ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let vec: vec4[f32] = vec4[f32](0.000000, 1.000000, 2.000000, 3.000000);
	let value: vec4[f32] = vec.xyz.yz.y.x.xxxx;
}
)");

			ExpectSPIRV(*shaderModule, R"(
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
[nzsl_version("1.0")]
module;

[entry(frag)]
fn main()
{
	let vec = vec4[f32](0.0, 1.0, 2.0, 3.0);
	vec.wyxz.bra.ts.x = 0.0;
	vec.zyxw.ar.xy.yx = vec2[f32](1.0, 0.0);
}
)";

			Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
			shaderModule = SanitizeModule(*shaderModule);

			ExpectGLSL(*shaderModule, R"(
void main()
{
	vec4 vec = vec4(0.000000, 1.000000, 2.000000, 3.000000);
	vec.wyxz.zxw.yx.x = 0.000000;
	vec.zyxw.wx.xy.yx = vec2(1.000000, 0.000000);
}
)");

			ExpectNZSL(*shaderModule, R"(
[entry(frag)]
fn main()
{
	let vec: vec4[f32] = vec4[f32](0.000000, 1.000000, 2.000000, 3.000000);
	vec.wyxz.zxw.yx.x = 0.000000;
	vec.zyxw.wx.xy.yx = vec2[f32](1.000000, 0.000000);
}
)");

			ExpectSPIRV(*shaderModule, R"(
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
