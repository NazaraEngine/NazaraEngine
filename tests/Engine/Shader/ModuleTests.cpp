#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/FilesystemModuleResolver.hpp>
#include <Nazara/Shader/LangWriter.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <catch2/catch.hpp>
#include <cctype>

TEST_CASE("Modules", "[Shader]")
{
	WHEN("using a simple module")
	{
		// UUID are required here to have a stable output
		std::string_view importedSource = R"(
[nzsl_version("1.0")]
[uuid("ad3aed6e-0619-4a26-b5ce-abc2ec0836c4")]
module SimpleModule;

[layout(std140)]
struct Data
{
	value: f32
}

[export]
[layout(std140)]
struct Block
{
	data: Data
}

struct Unused {}

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

		std::string_view shaderSource = R"(
[nzsl_version("1.0")]
module;

import SimpleModule;

external
{
	[binding(0)] block: uniform[Block]
}

[entry(frag)]
fn main(input: InputData) -> OutputData
{
	let output: OutputData;
	output.value = block.data.value * input.value;
	return output;
}
)";

		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(shaderSource);

		auto directoryModuleResolver = std::make_shared<Nz::FilesystemModuleResolver>();
		directoryModuleResolver->RegisterModule(importedSource);

		Nz::ShaderAst::SanitizeVisitor::Options sanitizeOpt;
		sanitizeOpt.moduleResolver = directoryModuleResolver;

		shaderModule = SanitizeModule(*shaderModule, sanitizeOpt);

		ExpectGLSL(*shaderModule, R"(
// Module ad3aed6e-0619-4a26-b5ce-abc2ec0836c4

struct Data__181c45e9
{
	float value;
};

struct Block__181c45e9
{
	Data__181c45e9 data;
};

struct InputData__181c45e9
{
	float value;
};

struct OutputData__181c45e9
{
	float value;
};

// Main file


layout(std140) uniform _NzBinding_block
{
	Data__181c45e9 data;
} block;


/**************** Inputs ****************/
in float _NzIn_value;

/*************** Outputs ***************/
out float _NzOut_value;

void main()
{
	InputData__181c45e9 input_;
	input_.value = _NzIn_value;
	
	OutputData__181c45e9 output_;
	output_.value = block.data.value * input_.value;
	
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
	[layout(std140)]
	struct Data
	{
		value: f32
	}
	
	[layout(std140)]
	struct Block
	{
		data: Data
	}
	
	struct InputData
	{
		value: f32
	}
	
	struct OutputData
	{
		value: f32
	}
	
}
alias Block = __181c45e9.Block;

alias InputData = __181c45e9.InputData;

alias OutputData = __181c45e9.OutputData;

external
{
	[set(0), binding(0)] block: uniform[__181c45e9.Block]
}

[entry(frag)]
fn main(input: InputData) -> OutputData
{
	let output: OutputData;
	output.value = block.data.value * input.value;
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
OpLoad
OpFMul
OpAccessChain
OpStore
OpLoad
OpReturn
OpFunctionEnd)");
	}

	WHEN("Using nested modules")
	{
		// UUID are required here to have a stable output
		std::string_view dataModule = R"(
[nzsl_version("1.0")]
[uuid("ad3aed6e-0619-4a26-b5ce-abc2ec0836c4")]
module Modules.Data;

fn dummy() {}

[export]
[layout(std140)]
struct Data
{
	value: f32
}
)";

		std::string_view blockModule = R"(
[nzsl_version("1.0")]
[uuid("7a548506-89e6-4944-897f-4f695a8bca01")]
module Modules.Block;

import Modules.Data;

[export]
[layout(std140)]
struct Block
{
	data: Data
}

struct Unused {}
)";

		std::string_view inputOutputModule = R"(
[nzsl_version("1.0")]
[uuid("e66c6e98-fc37-4390-a7e1-c81508ff8e49")]
module Modules.InputOutput;

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

		std::string_view shaderSource = R"(
[nzsl_version("1.0")]
module;

import Modules.Block;
import Modules.InputOutput;

external
{
	[binding(0)] block: uniform[Block]
}

[entry(frag)]
fn main(input: InputData) -> OutputData
{
	let output: OutputData;
	output.value = block.data.value * input.value;
	return output;
}
)";
		
		Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(shaderSource);

		auto directoryModuleResolver = std::make_shared<Nz::FilesystemModuleResolver>();
		directoryModuleResolver->RegisterModule(dataModule);
		directoryModuleResolver->RegisterModule(blockModule);
		directoryModuleResolver->RegisterModule(inputOutputModule);

		Nz::ShaderAst::SanitizeVisitor::Options sanitizeOpt;
		sanitizeOpt.moduleResolver = directoryModuleResolver;

		shaderModule = SanitizeModule(*shaderModule, sanitizeOpt);

		ExpectGLSL(*shaderModule, R"(
// Module ad3aed6e-0619-4a26-b5ce-abc2ec0836c4

struct Data__181c45e9
{
	float value;
};

// Module 7a548506-89e6-4944-897f-4f695a8bca01


struct Block__e528265d
{
	Data__181c45e9 data;
};

// Module e66c6e98-fc37-4390-a7e1-c81508ff8e49

struct InputData__26cce136
{
	float value;
};

struct OutputData__26cce136
{
	float value;
};

// Main file



layout(std140) uniform _NzBinding_block
{
	Data__181c45e9 data;
} block;


/**************** Inputs ****************/
in float _NzIn_value;

/*************** Outputs ***************/
out float _NzOut_value;

void main()
{
	InputData__26cce136 input_;
	input_.value = _NzIn_value;
	
	OutputData__26cce136 output_;
	output_.value = block.data.value * input_.value;
	
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
	[layout(std140)]
	struct Data
	{
		value: f32
	}
	
}
[nzsl_version("1.0")]
[uuid("7a548506-89e6-4944-897f-4f695a8bca01")]
module __e528265d
{
	alias Data = __181c45e9.Data;
	
	[layout(std140)]
	struct Block
	{
		data: Data
	}
	
}
[nzsl_version("1.0")]
[uuid("e66c6e98-fc37-4390-a7e1-c81508ff8e49")]
module __26cce136
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
alias Block = __e528265d.Block;

alias InputData = __26cce136.InputData;

alias OutputData = __26cce136.OutputData;

external
{
	[set(0), binding(0)] block: uniform[__e528265d.Block]
}

[entry(frag)]
fn main(input: InputData) -> OutputData
{
	let output: OutputData;
	output.value = block.data.value * input.value;
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
OpLoad
OpFMul
OpAccessChain
OpStore
OpLoad
OpReturn
OpFunctionEnd)");
	}
}
