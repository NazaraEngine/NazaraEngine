#include <Engine/Shader/ShaderUtils.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/LangWriter.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/SpirvPrinter.hpp>
#include <Nazara/Shader/SpirvWriter.hpp>
#include <catch2/catch.hpp>
#include <glslang/Public/ShaderLang.h>
#include <spirv-tools/libspirv.hpp>

namespace
{
	// Use OpenGL default minimal values (from https://www.khronos.org/registry/OpenGL-Refpages/es3.0/html/glGet.xhtml, https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGet.xhtml and https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/)
	const TBuiltInResource s_minResources = {
		8,      //< maxLights
		6,      //< maxClipPlanes
		32,     //< maxTextureUnits
		32,     //< maxTextureCoords
		16,     //< maxVertexAttribs
		1024,   //< maxVertexUniformComponents
		60,     //< maxVaryingFloats
		16,     //< maxVertexTextureImageUnits
		32,     //< maxCombinedTextureImageUnits
		16,     //< maxTextureImageUnits
		896,    //< maxFragmentUniformComponents
		4,      //< maxDrawBuffers
		256,    //< maxVertexUniformVectors
		15,     //< maxVaryingVectors
		224,    //< maxFragmentUniformVectors
		256,    //< maxVertexOutputVectors
		224,    //< maxFragmentInputVectors
		-8,     //< minProgramTexelOffset
		7,      //< maxProgramTexelOffset
		8,      //< maxClipDistances
		0xFFFF, //< maxComputeWorkGroupCountX
		0xFFFF, //< maxComputeWorkGroupCountY
		0xFFFF, //< maxComputeWorkGroupCountZ
		1024,   //< maxComputeWorkGroupSizeX
		1024,   //< maxComputeWorkGroupSizeY
		64,     //< maxComputeWorkGroupSizeZ
		1024,   //< maxComputeUniformComponents
		16,     //< maxComputeTextureImageUnits
		8,      //< maxComputeImageUniforms
		8,      //< maxComputeAtomicCounters
		1,      //< maxComputeAtomicCounterBuffers
		60,     //< maxVaryingComponents
		64,     //< maxVertexOutputComponents
		64,     //< maxGeometryInputComponents
		128,    //< maxGeometryOutputComponents
		128,    //< maxFragmentInputComponents
		8,      //< maxImageUnits
		8,      //< maxCombinedImageUnitsAndFragmentOutputs
		8,      //< maxCombinedShaderOutputResources
		0,      //< maxImageSamples
		0,      //< maxVertexImageUniforms
		0,      //< maxTessControlImageUniforms
		0,      //< maxTessEvaluationImageUniforms
		0,      //< maxGeometryImageUniforms
		8,      //< maxFragmentImageUniforms
		8,      //< maxCombinedImageUniforms
		16,     //< maxGeometryTextureImageUnits
		256,    //< maxGeometryOutputVertices
		1024,   //< maxGeometryTotalOutputComponents
		1024,   //< maxGeometryUniformComponents
		64,     //< maxGeometryVaryingComponents
		128,    //< maxTessControlInputComponents
		128,    //< maxTessControlOutputComponents
		16,     //< maxTessControlTextureImageUnits
		1024,   //< maxTessControlUniformComponents
		4096,   //< maxTessControlTotalOutputComponents
		128,    //< maxTessEvaluationInputComponents
		128,    //< maxTessEvaluationOutputComponents
		16,     //< maxTessEvaluationTextureImageUnits
		1024,   //< maxTessEvaluationUniformComponents
		120,    //< maxTessPatchComponents
		32,     //< maxPatchVertices
		64,     //< maxTessGenLevel
		16,     //< maxViewports
		0,      //< maxVertexAtomicCounters
		0,      //< maxTessControlAtomicCounters
		0,      //< maxTessEvaluationAtomicCounters
		0,      //< maxGeometryAtomicCounters
		8,      //< maxFragmentAtomicCounters
		8,      //< maxCombinedAtomicCounters
		1,      //< maxAtomicCounterBindings
		0,      //< maxVertexAtomicCounterBuffers
		0,      //< maxTessControlAtomicCounterBuffers
		0,      //< maxTessEvaluationAtomicCounterBuffers
		0,      //< maxGeometryAtomicCounterBuffers
		1,      //< maxFragmentAtomicCounterBuffers
		1,      //< maxCombinedAtomicCounterBuffers
		16384,  //< maxAtomicCounterBufferSize
		4,      //< maxTransformFeedbackBuffers
		64,     //< maxTransformFeedbackInterleavedComponents
		8,      //< maxCullDistances
		8,      //< maxCombinedClipAndCullDistances
		4,      //< maxSamples
		256,    //< maxMeshOutputVerticesNV
		512,    //< maxMeshOutputPrimitivesNV
		32,     //< maxMeshWorkGroupSizeX_NV
		1,      //< maxMeshWorkGroupSizeY_NV
		1,      //< maxMeshWorkGroupSizeZ_NV
		32,     //< maxTaskWorkGroupSizeX_NV
		1,      //< maxTaskWorkGroupSizeY_NV
		1,      //< maxTaskWorkGroupSizeZ_NV
		4,      //< maxMeshViewCountNV
		1,      //< maxDualSourceDrawBuffersEXT
		{       //< limits
			true, //< nonInductiveForLoops
			true, //< whileLoops
			true, //< doWhileLoops
			true, //< generalUniformIndexing
			true, //< generalAttributeMatrixVectorIndexing
			true, //< generalVaryingIndexing
			true, //< generalSamplerIndexing
			true, //< generalVariableIndexing
			true, //< generalConstantMatrixVectorIndexing
		}
	};
}

void ExpectingGLSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	expectedOutput = Nz::Trim(expectedOutput);

	Nz::GlslWriter writer;

	SECTION("Generating GLSL")
	{
		std::string output = writer.Generate(shader);

		WHEN("Validating expected code")
		{
			INFO("full GLSL output:\n" << output << "\nexcepted output:\n" << expectedOutput);
			REQUIRE(output.find(expectedOutput) != std::string::npos);
		}

		WHEN("Validating full GLSL code (using glslang)")
		{
			glslang::TShader shader(EShLangVertex);
			shader.setEnvInput(glslang::EShSourceGlsl, EShLangVertex, glslang::EShClientOpenGL, 300);
			shader.setEnvClient(glslang::EShClientOpenGL, glslang::EShTargetOpenGL_450);
			shader.setEnvTarget(glslang::EShTargetNone, static_cast<glslang::EShTargetLanguageVersion>(0));
			shader.setEntryPoint("main");

			const char* source = output.c_str();
			shader.setStrings(&source, 1);

			if (!shader.parse(&s_minResources, 100, false, static_cast<EShMessages>(EShMsgDefault | EShMsgKeepUncalled)))
			{
				INFO("full GLSL output:\n" << output << "\nerror:\n" << shader.getInfoLog());
				REQUIRE(false);
			}
		}
	}
}

void ExpectingNZSL(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	expectedOutput = Nz::Trim(expectedOutput);

	Nz::LangWriter writer;

	SECTION("Generating NZSL")
	{
		std::string output = writer.Generate(shader);

		WHEN("Validating expected code")
		{
			INFO("full NZSL output:\n" << output << "\nexcepted output:\n" << expectedOutput);
			REQUIRE(output.find(expectedOutput) != std::string::npos);
		}

		WHEN("Validating full NZSL code (by recompiling it)")
		{
			// validate NZSL by recompiling it
			REQUIRE_NOTHROW(Nz::ShaderLang::Parse(output));
		}
	}
}

void ExpectingSpirV(Nz::ShaderAst::Statement& shader, std::string_view expectedOutput)
{
	expectedOutput = Nz::Trim(expectedOutput);

	Nz::SpirvWriter writer;
	Nz::SpirvPrinter printer;

	Nz::SpirvPrinter::Settings settings;
	settings.printHeader = false;
	settings.printParameters = false;

	SECTION("Generating SPIRV")
	{
		auto spirv = writer.Generate(shader);
		std::string output = printer.Print(spirv.data(), spirv.size(), settings);

		WHEN("Validating expected code")
		{
			INFO("full SPIRV output:\n" << output << "\nexcepted output:\n" << expectedOutput);
			REQUIRE(output.find(expectedOutput) != std::string::npos);
		}

		WHEN("Validating full SPIRV code (using libspirv)")
		{
			// validate SPIRV with libspirv
			spvtools::SpirvTools spirvTools(spv_target_env::SPV_ENV_VULKAN_1_0);
			spirvTools.SetMessageConsumer([&](spv_message_level_t /*level*/, const char* /*source*/, const spv_position_t& /*position*/, const char* message)
			{
				std::string fullSpirv;
				if (!spirvTools.Disassemble(spirv, &fullSpirv))
					fullSpirv = "<failed to disassemble SPIRV>";

				UNSCOPED_INFO(fullSpirv + "\n" + message);
			});

			REQUIRE(spirvTools.Validate(spirv));
		}
	}
}
