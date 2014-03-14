// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <algorithm>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

NzUberShaderInstance* NzUberShaderPreprocessor::Get(const NzParameterList& parameters) const
{
	// Première étape, transformer les paramètres en un flag
	nzUInt32 flags = 0;
	for (auto it = m_flags.begin(); it != m_flags.end(); ++it)
	{
		if (parameters.HasParameter(it->first))
		{
			bool value;
			if (parameters.GetBooleanParameter(it->first, &value) && value)
				flags |= it->second;
		}
	}

	// Le shader fait-il partie du cache ?
	auto shaderIt = m_cache.find(flags);

	// Si non, il nous faut le construire
	if (shaderIt == m_cache.end())
	{
		try
		{
			NzErrorFlags errFlags(nzErrorFlag_Silent | nzErrorFlag_ThrowException);

			std::unique_ptr<NzShader> shader(new NzShader);
			shader->Create();

			for (unsigned int i = 0; i <= nzShaderStage_Max; ++i)
			{
				const Shader& shaderStage = m_shaders[i];

				if (shaderStage.present)
				{
					nzUInt32 stageFlags = 0;
					for (auto it = shaderStage.flags.begin(); it != shaderStage.flags.end(); ++it)
					{
						if (parameters.HasParameter(it->first))
						{
							bool value;
							if (parameters.GetBooleanParameter(it->first, &value) && value)
								stageFlags |= it->second;
						}
					}

					auto stageIt = shaderStage.cache.find(stageFlags);
					if (stageIt == shaderStage.cache.end())
					{
						NzShaderStage stage;
						stage.Create(static_cast<nzShaderStage>(i));

						unsigned int glslVersion = NzOpenGL::GetGLSLVersion();

						NzStringStream code;
						code << "#version " << glslVersion << "\n\n";

						code << "#define GLSL_VERSION " << glslVersion << "\n\n";

						code << "#define EARLY_FRAGMENT_TEST " << (glslVersion >= 420 || NzOpenGL::IsSupported(nzOpenGLExtension_Shader_ImageLoadStore)) << "\n\n";

						for (auto it = shaderStage.flags.begin(); it != shaderStage.flags.end(); ++it)
							code << "#define " << it->first << ' ' << ((stageFlags & it->second) ? '1' : '0') << '\n';

						code << "\n#line 1\n";
						code << shaderStage.source;

						stage.SetSource(code);
						stage.Compile();

						shader->AttachStage(static_cast<nzShaderStage>(i), stage);

						shaderStage.cache.emplace(flags, std::move(stage));
					}
					else
						shader->AttachStage(static_cast<nzShaderStage>(i), stageIt->second);
				}
			}

			shader->Link();

			auto pair = m_cache.emplace(flags, shader.get());
			shader.release();

			return &(pair.first)->second;
		}
		catch (const std::exception& e)
		{
			NzErrorFlags errFlags(nzErrorFlag_ThrowExceptionDisabled);

			NazaraError("Failed to build uber shader instance: " + NzError::GetLastError());
			throw;
		}

	}
	else
		return &shaderIt->second;
}

void NzUberShaderPreprocessor::SetShader(nzShaderStage stage, const NzString& source, const NzString& flagString)
{
    Shader& shader = m_shaders[stage];
    shader.present = true;
    shader.source = source;

    std::vector<NzString> flags;
    flagString.Split(flags, ' ');

    for (NzString& flag : flags)
	{
		auto it = m_flags.find(flag);
		if (it == m_flags.end())
			m_flags[flag] = 1U << m_flags.size();

		auto it2 = shader.flags.find(flag);
		if (it2 == shader.flags.end())
			shader.flags[flag] = 1U << shader.flags.size();
	}
}

bool NzUberShaderPreprocessor::SetShaderFromFile(nzShaderStage stage, const NzString& filePath, const NzString& flags)
{
	NzFile file(filePath);
	if (!file.Open(NzFile::ReadOnly | NzFile::Text))
	{
		NazaraError("Failed to open \"" + filePath + '"');
		return false;
	}

	unsigned int length = static_cast<unsigned int>(file.GetSize());

	NzString source(length, '\0');

	if (file.Read(&source[0], length) != length)
	{
		NazaraError("Failed to read program file");
		return false;
	}

	file.Close();

	SetShader(stage, source, flags);
	return true;
}
