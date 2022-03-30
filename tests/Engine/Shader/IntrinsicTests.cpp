#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("intrinsics", "[Shader]")
{
	WHEN("using intrinsics")
	{
		std::string_view nzslSource = R"(
[nzsl_version("1.0")]
module;

external
{
	[set(0), binding(0)] tex: sampler2D[f32]
}

[entry(frag)]
fn main()
{
	let f1 = 42.0;
	let f2 = 1337.0;
	let i1 = 42;
	let i2 = 1337;
	let uv = vec2[f32](0.0, 1.0);
	let v1 = vec3[f32](0.0, 1.0, 2.0);
	let v2 = vec3[f32](2.0, 1.0, 0.0);

	let crossResult = cross(v1, v2);
	let dotResult = dot(v1, v2);
	let expResult1 = exp(v1);
	let expResult2 = exp(f1);
	let lengthResult = length(v1);
	let maxResult1 = max(f1, f2);
	let maxResult2 = max(i1, i2);
	let maxResult3 = max(v1, v2);
	let minResult1 = min(f1, f2);
	let minResult2 = min(i1, i2);
	let minResult3 = min(v1, v2);
	let normalizeResult = normalize(v1);
	let powResult1 = pow(f1, f2);
	let powResult2 = pow(v1, v2);
	let reflectResult = reflect(v1, v2);
	let sampleResult = tex.Sample(uv);
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(nzslSource);
		shaderModule = SanitizeModule(*shaderModule);

		ExpectGLSL(*shaderModule, R"(
void main()
{
	float f1 = 42.000000;
	float f2 = 1337.000000;
	int i1 = 42;
	int i2 = 1337;
	vec2 uv = vec2(0.000000, 1.000000);
	vec3 v1 = vec3(0.000000, 1.000000, 2.000000);
	vec3 v2 = vec3(2.000000, 1.000000, 0.000000);
	vec3 crossResult = cross(v1, v2);
	float dotResult = dot(v1, v2);
	vec3 expResult1 = exp(v1);
	float expResult2 = exp(f1);
	float lengthResult = length(v1);
	float maxResult1 = max(f1, f2);
	int maxResult2 = max(i1, i2);
	vec3 maxResult3 = max(v1, v2);
	float minResult1 = min(f1, f2);
	int minResult2 = min(i1, i2);
	vec3 minResult3 = min(v1, v2);
	vec3 normalizeResult = normalize(v1);
	float powResult1 = pow(f1, f2);
	vec3 powResult2 = pow(v1, v2);
	vec3 reflectResult = reflect(v1, v2);
	vec4 sampleResult = texture(tex, uv);
}
)");

		ExpectNZSL(*shaderModule, R"(
fn main()
{
	let f1: f32 = 42.000000;
	let f2: f32 = 1337.000000;
	let i1: i32 = 42;
	let i2: i32 = 1337;
	let uv: vec2[f32] = vec2[f32](0.000000, 1.000000);
	let v1: vec3[f32] = vec3[f32](0.000000, 1.000000, 2.000000);
	let v2: vec3[f32] = vec3[f32](2.000000, 1.000000, 0.000000);
	let crossResult: vec3[f32] = cross(v1, v2);
	let dotResult: f32 = dot(v1, v2);
	let expResult1: vec3[f32] = exp(v1);
	let expResult2: f32 = exp(f1);
	let lengthResult: f32 = length(v1);
	let maxResult1: f32 = max(f1, f2);
	let maxResult2: i32 = max(i1, i2);
	let maxResult3: vec3[f32] = max(v1, v2);
	let minResult1: f32 = min(f1, f2);
	let minResult2: i32 = min(i1, i2);
	let minResult3: vec3[f32] = min(v1, v2);
	let normalizeResult: vec3[f32] = normalize(v1);
	let powResult1: f32 = pow(f1, f2);
	let powResult2: vec3[f32] = pow(v1, v2);
	let reflectResult: vec3[f32] = reflect(v1, v2);
	let sampleResult: vec4[f32] = tex.Sample(uv);
}
)");

		ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpVariable
OpStore
OpStore
OpStore
OpStore
OpCompositeConstruct
OpStore
OpCompositeConstruct
OpStore
OpCompositeConstruct
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpDot
OpStore
OpLoad
OpExtInst
OpStore
OpLoad
OpExtInst
OpStore
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpExtInst
OpStore
OpLoad
OpLoad
OpImageSampleImplicitLod
OpStore
OpReturn
OpFunctionEnd)");
	}
	
}
