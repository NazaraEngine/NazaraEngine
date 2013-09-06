// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Directory.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/ShaderProgramManager.hpp>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	struct ParamsHash
	{
		std::size_t operator()(const NzShaderProgramManagerParams& params) const
		{
			static_assert(nzShaderTarget_Max < 0x4, "Maximum shader target takes more than 2 bits");

			std::size_t h = (params.target << 0) | // 2 bits
			                (params.flags  << 2);  // 8 bits

			switch (params.target)
			{
				case nzShaderTarget_FullscreenQuad:
					h |= (params.fullscreenQuad.alphaMapping   << 10) | // 1 bit
					     (params.fullscreenQuad.alphaTest      << 11) | // 1 bit
					     (params.fullscreenQuad.diffuseMapping << 12);  // 1 bit
					break;

				case nzShaderTarget_Model:
					h |= (params.model.alphaMapping    << 10) | // 1 bit
					     (params.model.alphaTest       << 11) | // 1 bit
					     (params.model.diffuseMapping  << 12) | // 1 bit
					     (params.model.emissiveMapping << 13) | // 1 bit
					     (params.model.lighting        << 14) | // 1 bit
					     (params.model.normalMapping   << 15) | // 1 bit
					     (params.model.parallaxMapping << 16) | // 1 bit
					     (params.model.specularMapping << 17);  // 1 bit
					break;

				case nzShaderTarget_None:
					break;

				case nzShaderTarget_Sprite:
					h |= (params.sprite.alphaMapping   << 10) | // 1 bit
					     (params.sprite.alphaTest      << 11) | // 1 bit
					     (params.sprite.diffuseMapping << 12);  // 1 bit
					break;
			}

			return h;
		}
	};

	struct ParamsEquality
	{
		bool operator()(const NzShaderProgramManagerParams& first, const NzShaderProgramManagerParams& second) const
		{
			if (first.target != second.target || first.flags != second.flags)
				return false;

			switch (first.target)
			{
				case nzShaderTarget_FullscreenQuad:
					return std::memcmp(&first.fullscreenQuad, &second.fullscreenQuad, sizeof(NzShaderProgramManagerParams::FullscreenQuad)) == 0;

				case nzShaderTarget_Model:
					return std::memcmp(&first.model, &second.model, sizeof(NzShaderProgramManagerParams::Model)) == 0;

				case nzShaderTarget_None:
					return true;

				case nzShaderTarget_Sprite:
					return std::memcmp(&first.sprite, &second.sprite, sizeof(NzShaderProgramManagerParams::Sprite)) == 0;
			}

			return false;
		}
	};

	std::unordered_map<NzShaderProgramManagerParams, NzShaderProgramRef, ParamsHash, ParamsEquality> s_programs;
	NzString s_cacheDirectory("shaderCache");
	bool s_cacheEnabled = false;
	bool s_earlyFragmentTest;
	bool s_glsl140;
	//bool s_loadCachedPrograms = true;
	unsigned int s_glslVersion;
}

const NzShaderProgram* NzShaderProgramManager::Get(const NzShaderProgramManagerParams& params)
{
	auto it = s_programs.find(params);
	if (it == s_programs.end())
	{
		// Alors nous générons le programme
		std::unique_ptr<NzShaderProgram> program;

		if (s_cacheEnabled)
		{
			NzString programFileName = NzNumberToString(ParamsHash()(params), 36) + ".nsb"; // Nazara Shader Binary, très original, je sais
			NazaraDebug("Checking cache for program file \"" + programFileName + "\"...");

			NzFile shaderFile(s_cacheDirectory + NAZARA_DIRECTORY_SEPARATOR + programFileName);
			if (shaderFile.Open(NzFile::ReadOnly))
			{
				NazaraDebug("File found");

				unsigned int size = shaderFile.GetSize();

				NzByteArray binary;
				binary.Resize(size);

				if (shaderFile.Read(&binary[0], size) != size)
				{
					NazaraError("Failed to read program binary");
					return false;
				}

				shaderFile.Close();

				program.reset(new NzShaderProgram);
				if (!program->LoadFromBinary(binary))
				{
					NazaraWarning("Program \"" + programFileName + "\" loading failed, this is mostly due to a driver/video card "
					              "update or a file corruption, regenerating program...");

					program.reset(GenerateProgram(params));
				}
			}
			else
			{
				if (shaderFile.Exists())
					NazaraWarning("Program file exists but couldn't be opened");

				program.reset(GenerateProgram(params));
				if (program)
				{
					if (program->IsBinaryRetrievable())
					{
						NazaraDebug("Program \"" + programFileName + "\" (re)generated, saving it into program cache directory...");

						NzByteArray programBinary = program->GetBinary();
						if (!programBinary.IsEmpty())
						{
							if (!shaderFile.Open(NzFile::Truncate | NzFile::WriteOnly) || !shaderFile.Write(programBinary))
								NazaraWarning("Failed to save program binary to file \"" + programFileName + "\": " + NzGetLastSystemError());
						}
						else
							NazaraWarning("Failed to retrieve shader program binary");
					}
				}
			}
		}
		else
			program.reset(GenerateProgram(params));

		if (program)
			s_programs[params] = program.get();
		else
		{
			NazaraWarning("Failed to build program, using default one...");

			NzShaderProgramManagerParams defaultParams;
			defaultParams.flags = params.flags;
			defaultParams.target = nzShaderTarget_None;

			program.reset(s_programs[defaultParams]); // Shader par défaut
		}

		return program.release();
	}
	else
		return it->second;
}

NzString NzShaderProgramManager::BuildFragmentCode(const NzShaderProgramManagerParams& params)
{
	#ifdef NAZARA_DEBUG
	if (params.target > nzShaderTarget_Max)
	{
		NazaraError("Shader target out of enum");
		return NzString();
	}
	#endif

	NzString source;

	/********************Header********************/
	source.Reserve(14 + 24 + 24 + 26 + 1);
	source += "#version ";
	source += NzString::Number(s_glslVersion);
	source += "\n\n";

	source += "#define FLAG_DEFERRED ";
	source += (params.flags & nzShaderFlags_Deferred) ? '1' : '0';
	source += '\n';

	source += "#define FLAG_FLIP_UVS ";
	source += (params.flags & nzShaderFlags_FlipUVs) ? '1' : '0';
	source += '\n';

	source += "#define FLAG_INSTANCING ";
	source += (params.flags & nzShaderFlags_Instancing) ? '1' : '0';
	source += '\n';

	source += '\n';

	switch (params.target)
	{
		case nzShaderTarget_FullscreenQuad:
		{
			char coreFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/FullscreenQuad/core.frag.h>
			};

			char compatibilityFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/FullscreenQuad/compatibility.frag.h>
			};

			const char* shaderSource = (s_glsl140) ? coreFragmentShader : compatibilityFragmentShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreFragmentShader) : sizeof(compatibilityFragmentShader);


			source.Reserve(source.GetCapacity() + 34 + 24 + 21 + 26 + 1 + shaderSourceSize);

			// "discard" ne s'entend pas bien avec les early fragment tests
			if (s_earlyFragmentTest && !params.fullscreenQuad.alphaMapping)
				source += "layout(early_fragment_tests) in;\n\n";

			source += "#define ALPHA_MAPPING ";
			source += (params.fullscreenQuad.alphaMapping) ? '1' : '0';
			source += '\n';

			source += "#define ALPHA_TEST ";
			source += (params.fullscreenQuad.alphaTest) ? '1' : '0';
			source += '\n';

			source += "#define DIFFUSE_MAPPING ";
			source += (params.fullscreenQuad.diffuseMapping) ? '1' : '0';
			source += '\n';

			source += '\n';

			source.Append(shaderSource, shaderSourceSize);
			break;
		}

		case nzShaderTarget_Model:
		{
			char coreFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/Model/core.frag.h>
			};

			char compatibilityFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/Model/compatibility.frag.h>
			};

			const char* shaderSource = (s_glsl140) ? coreFragmentShader : compatibilityFragmentShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreFragmentShader) : sizeof(compatibilityFragmentShader);

			source.Reserve(source.GetCapacity() + 34 + 24 + 21 + 26 + 27 + 19 + 25 + 27 + 27 + 1 + shaderSourceSize);

			if (s_earlyFragmentTest && !params.model.alphaMapping)
				source += "layout(early_fragment_tests) in;\n\n";

			source += "#define ALPHA_MAPPING ";
			source += (params.model.alphaMapping) ? '1' : '0';
			source += '\n';

			source += "#define ALPHA_TEST ";
			source += (params.model.alphaTest) ? '1' : '0';
			source += '\n';

			source += "#define DIFFUSE_MAPPING ";
			source += (params.model.diffuseMapping) ? '1' : '0';
			source += '\n';

			source += "#define EMISSIVE_MAPPING ";
			source += (params.model.emissiveMapping) ? '1' : '0';
			source += '\n';

			source += "#define LIGHTING ";
			source += (params.model.lighting) ? '1' : '0';
			source += '\n';

			source += "#define NORMAL_MAPPING ";
			source += (params.model.normalMapping) ? '1' : '0';
			source += '\n';

			source += "#define PARALLAX_MAPPING ";
			source += (params.model.parallaxMapping) ? '1' : '0';
			source += '\n';

			source += "#define SPECULAR_MAPPING ";
			source += (params.model.specularMapping) ? '1' : '0';
			source += '\n';

			source += '\n';

			source.Append(shaderSource, shaderSourceSize);
			break;
		}

		case nzShaderTarget_None:
		{
			char coreFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/None/core.frag.h>
			};

			char compatibilityFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/None/compatibility.frag.h>
			};

			const char* shaderSource = (s_glsl140) ? coreFragmentShader : compatibilityFragmentShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreFragmentShader) : sizeof(compatibilityFragmentShader);

			source.Reserve(source.GetCapacity() + 34 + shaderSourceSize);

			if (s_earlyFragmentTest)
				source += "layout(early_fragment_tests) in;\n\n";

			source.Append(shaderSource, shaderSourceSize);
			break;
		}

		case nzShaderTarget_Sprite:
		{
			char coreFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/Sprite/core.frag.h>
			};

			char compatibilityFragmentShader[] = {
				#include <Nazara/Renderer/Shaders/Sprite/compatibility.frag.h>
			};

			const char* shaderSource = (s_glsl140) ? coreFragmentShader : compatibilityFragmentShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreFragmentShader) : sizeof(compatibilityFragmentShader);

			source.Reserve(source.GetCapacity() + 34 + 24 + 21 + 26 + 1 + shaderSourceSize);

			// "discard" ne s'entend pas bien avec les early fragment tests
			if (s_earlyFragmentTest && !params.sprite.alphaMapping)
				source += "layout(early_fragment_tests) in;\n\n";

			source += "#define ALPHA_MAPPING ";
			source += (params.sprite.alphaMapping) ? '1' : '0';
			source += '\n';

			source += "#define ALPHA_TEST ";
			source += (params.sprite.alphaTest) ? '1' : '0';
			source += '\n';

			source += "#define DIFFUSE_MAPPING ";
			source += (params.sprite.diffuseMapping) ? '1' : '0';
			source += '\n';

			source += '\n';

			source.Append(shaderSource, shaderSourceSize);
			break;
		}
	}

	return source;
}

NzString NzShaderProgramManager::BuildVertexCode(const NzShaderProgramManagerParams& params)
{
	#ifdef NAZARA_DEBUG
	if (params.target > nzShaderTarget_Max)
	{
		NazaraError("Shader target out of enum");
		return NzString();
	}
	#endif

	NzString source;

	/********************Header********************/
	source.Reserve(14 + 24 + 24 + 26 + 1);
	source += "#version ";
	source += NzString::Number(s_glslVersion);
	source += "\n\n";

	source += "#define FLAG_DEFERRED ";
	source += (params.flags & nzShaderFlags_Deferred) ? '1' : '0';
	source += '\n';

	source += "#define FLAG_FLIP_UVS ";
	source += (params.flags & nzShaderFlags_FlipUVs) ? '1' : '0';
	source += '\n';

	source += "#define FLAG_INSTANCING ";
	source += (params.flags & nzShaderFlags_Instancing) ? '1' : '0';
	source += '\n';

	source += '\n';

	switch (params.target)
	{
		case nzShaderTarget_FullscreenQuad:
		{
			char coreVertexShader[] = {
				#include <Nazara/Renderer/Shaders/FullscreenQuad/core.vert.h>
			};

			char compatibilityVertexShader[] = {
				#include <Nazara/Renderer/Shaders/FullscreenQuad/compatibility.vert.h>
			};

			const char* shaderSource = (s_glsl140) ? coreVertexShader : compatibilityVertexShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreVertexShader) : sizeof(compatibilityVertexShader);

			source.Reserve(source.GetCapacity() + 24 + 21 + 26 + 1 + shaderSourceSize);

			source += "#define ALPHA_MAPPING ";
			source += (params.fullscreenQuad.alphaMapping) ? '1' : '0';
			source += '\n';

			source += "#define ALPHA_TEST ";
			source += (params.fullscreenQuad.alphaTest) ? '1' : '0';
			source += '\n';

			source += "#define DIFFUSE_MAPPING ";
			source += (params.fullscreenQuad.diffuseMapping) ? '1' : '0';
			source += '\n';

			source += '\n';

			source.Append(shaderSource, shaderSourceSize);
			break;
		}

		case nzShaderTarget_Model:
		{
			char coreVertexShader[] = {
				#include <Nazara/Renderer/Shaders/Model/core.vert.h>
			};

			char compatibilityVertexShader[] = {
				#include <Nazara/Renderer/Shaders/Model/compatibility.vert.h>
			};

			const char* shaderSource = (s_glsl140) ? coreVertexShader : compatibilityVertexShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreVertexShader) : sizeof(compatibilityVertexShader);

			source.Reserve(source.GetCapacity() + 24 + 21 + 26 + 27 + 19 + 25 + 27 + 27 + 1 + shaderSourceSize);

			source += "#define ALPHA_MAPPING ";
			source += (params.model.alphaMapping) ? '1' : '0';
			source += '\n';

			source += "#define ALPHA_TEST ";
			source += (params.model.alphaTest) ? '1' : '0';
			source += '\n';

			source += "#define DIFFUSE_MAPPING ";
			source += (params.model.diffuseMapping) ? '1' : '0';
			source += '\n';

			source += "#define EMISSIVE_MAPPING ";
			source += (params.model.emissiveMapping) ? '1' : '0';
			source += '\n';

			source += "#define LIGHTING ";
			source += (params.model.lighting) ? '1' : '0';
			source += '\n';

			source += "#define NORMAL_MAPPING ";
			source += (params.model.normalMapping) ? '1' : '0';
			source += '\n';

			source += "#define PARALLAX_MAPPING ";
			source += (params.model.parallaxMapping) ? '1' : '0';
			source += '\n';

			source += "#define SPECULAR_MAPPING ";
			source += (params.model.specularMapping) ? '1' : '0';
			source += '\n';

			source += '\n';

			source.Append(shaderSource, shaderSourceSize);
			break;
		}

		case nzShaderTarget_None:
		{
			char coreVertexShader[] = {
				#include <Nazara/Renderer/Shaders/None/core.vert.h>
			};

			char compatibilityVertexShader[] = {
				#include <Nazara/Renderer/Shaders/None/compatibility.vert.h>
			};

			const char* shaderSource = (s_glsl140) ? coreVertexShader : compatibilityVertexShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreVertexShader) : sizeof(compatibilityVertexShader);

			source.Append(shaderSource, shaderSourceSize);
			break;
		}

		case nzShaderTarget_Sprite:
		{
			char coreVertexShader[] = {
				#include <Nazara/Renderer/Shaders/Sprite/core.vert.h>
			};

			char compatibilityVertexShader[] = {
				#include <Nazara/Renderer/Shaders/Sprite/compatibility.vert.h>
			};

			const char* shaderSource = (s_glsl140) ? coreVertexShader : compatibilityVertexShader;
			unsigned int shaderSourceSize = (s_glsl140) ? sizeof(coreVertexShader) : sizeof(compatibilityVertexShader);

			source.Reserve(source.GetCapacity() + 24 + 21 + 26 + 1 + shaderSourceSize);

			source += "#define ALPHA_MAPPING ";
			source += (params.fullscreenQuad.alphaMapping) ? '1' : '0';
			source += '\n';

			source += "#define ALPHA_TEST ";
			source += (params.fullscreenQuad.alphaTest) ? '1' : '0';
			source += '\n';

			source += "#define DIFFUSE_MAPPING ";
			source += (params.fullscreenQuad.diffuseMapping) ? '1' : '0';
			source += '\n';

			source += '\n';

			source.Append(shaderSource, shaderSourceSize);
			break;
		}
	}

	return source;
}

NzShaderProgram* NzShaderProgramManager::GenerateProgram(const NzShaderProgramManagerParams& params)
{
	std::unique_ptr<NzShaderProgram> program(new NzShaderProgram);
	program->SetPersistent(false);

	if (!program->Create(nzShaderLanguage_GLSL))
	{
		NazaraError("Failed to create program");
		return nullptr;
	}

	NzString fragmentSource = BuildFragmentCode(params);
	if (!program->LoadShader(nzShaderType_Fragment, fragmentSource))
	{
		NazaraError("Failed to load fragment shader: " + program->GetLog());
		NazaraNotice("Source:\n" + fragmentSource);
		return nullptr;
	}

	NzString vertexSource = BuildVertexCode(params);
	if (!program->LoadShader(nzShaderType_Vertex, vertexSource))
	{
		NazaraError("Failed to load vertex shader: " + program->GetLog());
		NazaraNotice("Source:\n" + vertexSource);
		return nullptr;
	}

	if (!program->Compile())
	{
		NazaraError("Failed to compile program: " + program->GetLog());
		return nullptr;
	}

	return program.release();
}

bool NzShaderProgramManager::Initialize()
{
	s_glslVersion = NzOpenGL::GetGLSLVersion();
	s_earlyFragmentTest = (s_glslVersion >= 420 || NzOpenGL::IsSupported(nzOpenGLExtension_Shader_ImageLoadStore));
	s_glsl140 = (s_glslVersion >= 140);

	NzShaderProgramManagerParams params;
	params.target = nzShaderTarget_None;
	for (unsigned int i = 0; i <= nzShaderFlags_Max; ++i)
	{
		params.flags = i;

		NzShaderProgram* program = GenerateProgram(params);
		if (!program)
		{
			NazaraError("Failed to generate default program (flags: 0x" + NzString::Number(i, 16) + ')');
			Uninitialize();
			return false;
		}

		s_programs[params] = program;
	}

	/*if (s_loadCachedPrograms)
	{
		NzDirectory cacheDirectory(s_cacheDirectory);
		cacheDirectory.SetPattern("*.nsb");

		if (cacheDirectory.Open())
		{
			while (cacheDirectory.NextResult(true))
			{
				long long hash;
				if (cacheDirectory.GetResultName().SubStringTo(".nsb", -1, true, false).ToInteger(&hash, 32))
				{
					std::size_t hashCode = static_cast<std::size_t>(hash);

					if (s_programs.find(hashCode) == s_programs.end())
					{
						NzFile shaderFile(cacheDirectory.GetResultPath());
						if (shaderFile.Open(NzFile::ReadOnly))
						{
							unsigned int size = cacheDirectory.GetResultSize();

							NzByteArray binary;
							binary.Resize(size);

							if (shaderFile.Read(&binary[0], size) != size)
							{
								NazaraError("Failed to read program binary");
								return false;
							}

							shaderFile.Close();

							std::unique_ptr<NzShaderProgram> program(new NzShaderProgram);
							if (program->LoadFromBinary(binary))
								s_programs[hashCode] = binary.release();
							else
								NazaraWarning("Program binary \"" + cacheDirectory.GetResultName() + "\" loading failed, this is mostly due to a driver/video card "
											  "update or a file corruption, regenerating program...");							}
					}
				}
				else
					NazaraWarning("Failed to parse program file name (" + cacheDirectory.GetResultName() + ')');
			}
		}
		else if (cacheDirectory.Exists())
			NazaraWarning("Failed to open shader cache directory");
	}*/

	return true;
}

void NzShaderProgramManager::Uninitialize()
{
	s_programs.clear();
}
