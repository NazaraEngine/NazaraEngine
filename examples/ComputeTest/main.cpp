#include <Nazara/Core.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Parser.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

const char shaderSource[] = R"(
[nzsl_version("1.0")]
module;

external
{
	[binding(0)] input_tex: texture2D[f32, readonly, rgba8],
	[binding(1)] output_tex: texture2D[f32, writeonly, rgba8]
}

struct Input
{
	[builtin(global_invocation_indices)] global_invocation_id: vec3[u32]
}

[entry(compute)]
[workgroup(16, 16, 1)]
fn main(input: Input)
{
	let indices = vec2[i32](input.global_invocation_id.xy);
	let color = input_tex.Read(indices);
	output_tex.Write(indices, color);
}
)";

struct ComputePipeline
{
	std::shared_ptr<Nz::RenderPipelineLayout> layout;
	std::shared_ptr<Nz::ComputePipeline> pipeline;
};

ComputePipeline BuildComputePipeline(Nz::RenderDevice& device)
{
	try
	{
		nzsl::Ast::ModulePtr shaderModule = nzsl::Parse(std::string_view(shaderSource, sizeof(shaderSource)));
		if (!shaderModule)
		{
			std::cout << "Failed to parse shader module" << std::endl;
			std::abort();
		}

		nzsl::ShaderWriter::States states;
		states.optimize = true;

		auto computeShader = device.InstantiateShaderModule(nzsl::ShaderStageType::Compute, *shaderModule, states);
		if (!computeShader)
		{
			std::cout << "Failed to instantiate shader" << std::endl;
			std::abort();
		}

		Nz::RenderPipelineLayoutInfo computePipelineLayoutInfo;

		auto& inputBinding = computePipelineLayoutInfo.bindings.emplace_back();
		inputBinding.setIndex = 0;
		inputBinding.bindingIndex = 0;
		inputBinding.shaderStageFlags = nzsl::ShaderStageType::Compute;
		inputBinding.type = Nz::ShaderBindingType::Texture;

		auto& outputBinding = computePipelineLayoutInfo.bindings.emplace_back();
		outputBinding.setIndex = 0;
		outputBinding.bindingIndex = 1;
		outputBinding.shaderStageFlags = nzsl::ShaderStageType::Compute;
		outputBinding.type = Nz::ShaderBindingType::Texture;

		std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout = device.InstantiateRenderPipelineLayout(computePipelineLayoutInfo);

		Nz::ComputePipelineInfo computePipelineInfo;
		computePipelineInfo.pipelineLayout = pipelineLayout;
		computePipelineInfo.shaderModule = computeShader;

		std::shared_ptr<Nz::ComputePipeline> pipeline = device.InstantiateComputePipeline(computePipelineInfo);
		if (!pipeline)
		{
			std::cout << "Failed to instantiate compute pipeline" << std::endl;
			std::abort();
		}

		ComputePipeline result;
		result.layout = std::move(pipelineLayout);
		result.pipeline = std::move(pipeline);
		return result;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::abort();
	}
}

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Renderer> nazara(rendererConfig);

	Nz::RenderDeviceFeatures enabledFeatures;
	enabledFeatures.computeShaders = true;
	enabledFeatures.textureRead = true;
	enabledFeatures.textureWrite = true;

	std::shared_ptr<Nz::RenderDevice> device = Nz::Renderer::Instance()->InstanciateRenderDevice(0, enabledFeatures);

	ComputePipeline result = BuildComputePipeline(*device);


	return EXIT_SUCCESS;
}
