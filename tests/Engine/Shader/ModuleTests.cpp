#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("Modules", "[Shader]")
{
	WHEN("using a simple module")
	{
		std::string_view importedSource = R"(
[nzsl_version("1.0")]
[uuid("ad3aed6e-0619-4a26-b5ce-abc2ec0836c4")]
module;

[export]
struct InputData
{
	value: f32
}

[export]
struct OutputData
{
	value: f32
}
)";

		Nz::ShaderAst::ModulePtr importedModule = Nz::ShaderLang::Parse(importedSource);

		std::string_view shaderSource = R"(
[nzsl_version("1.0")]
module;

import SimpleModule;

[entry(frag)]
fn main(input: InputData) -> OutputData
{
	let output: OutputData;
	output.value = input.value;
	return output;
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(shaderSource);

		Nz::ShaderAst::SanitizeVisitor::Options sanitizeOpt;
		sanitizeOpt.moduleCallback = [&](const std::vector<std::string>& modulePath) -> Nz::ShaderAst::ModulePtr
		{
			REQUIRE(modulePath.size() == 1);
			REQUIRE(modulePath[0] == "SimpleModule");

			return importedModule;
		};

		REQUIRE_NOTHROW(shaderModule = Nz::ShaderAst::Sanitize(*shaderModule, sanitizeOpt));

		ExpectGLSL(*shaderModule, R"(
struct InputData__181c45e9
{
	float value;
};

struct OutputData__181c45e9
{
	float value;
};

/**************** Inputs ****************/
in float _NzIn_value;

/*************** Outputs ***************/
out float _NzOut_value;

void main()
{
	InputData__181c45e9 input_;
	input_.value = _NzIn_value;
	
	OutputData__181c45e9 output_;
	output_.value = input_.value;
	
	_NzOut_value = output_.value;
	return;
}
)");

		ExpectNZSL(*shaderModule, R"(
[nzsl_version("1.0")]
module;

[nzsl_version("1.0")]
[uuid("ad3aed6e-0619-4a26-b5ce-abc2ec0836c4")]
module __181c45e9
{
	struct InputData
	{
		value: f32
	}
	
	struct OutputData
	{
		value: f32
	}
	
}
alias InputData = __181c45e9.InputData;

alias OutputData = __181c45e9.OutputData;

[entry(frag)]
fn main(input: InputData) -> OutputData
{
	let output: OutputData;
	output.value = input.value;
	return output;
}
)");

		ExpectSPIRV(*shaderModule, R"(
OpFunction
OpLabel
OpVariable
OpVariable
OpAccessChain
OpLoad
OpAccessChain
OpStore
OpLoad
OpReturn
OpFunctionEnd)");
	}
}
