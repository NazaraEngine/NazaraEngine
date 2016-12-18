// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <algorithm>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	UberShaderPreprocessor::~UberShaderPreprocessor()
	{
		OnUberShaderPreprocessorRelease(this);
	}

	UberShaderInstance* UberShaderPreprocessor::Get(const ParameterList& parameters) const
	{
		// Première étape, transformer les paramètres en un flag
		UInt32 flags = 0;
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
				// Une exception sera lancée à la moindre erreur et celle-ci ne sera pas enregistrée dans le log (car traitée dans le bloc catch)
				ErrorFlags errFlags(ErrorFlag_Silent | ErrorFlag_ThrowException, true);

				ShaderRef shader = Shader::New();
				shader->Create();

				for (unsigned int i = 0; i <= ShaderStageType_Max; ++i)
				{
					const CachedShader& shaderStage = m_shaders[i];

					// Le shader stage est-il activé dans cette version du shader ?
					if (shaderStage.present && (flags & shaderStage.requiredFlags) == shaderStage.requiredFlags)
					{
						UInt32 stageFlags = 0;
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
							ShaderStage stage;
							stage.Create(static_cast<ShaderStageType>(i));

							unsigned int glslVersion = OpenGL::GetGLSLVersion();

							StringStream code;
							code << "#version " << glslVersion << "\n\n";

							code << "#define GLSL_VERSION " << glslVersion << "\n\n";

							code << "#define EARLY_FRAGMENT_TESTS " << ((glslVersion >= 420 || OpenGL::IsSupported(OpenGLExtension_Shader_ImageLoadStore)) ? '1' : '0') << "\n\n";

							for (auto it = shaderStage.flags.begin(); it != shaderStage.flags.end(); ++it)
								code << "#define " << it->first << ' ' << ((stageFlags & it->second) ? '1' : '0') << '\n';

							code << "\n#line 1\n"; // Pour que les éventuelles erreurs du shader se réfèrent à la bonne ligne
							code << shaderStage.source;

							stage.SetSource(code);

							try
							{
								stage.Compile();
							}
							catch (const std::exception&)
							{
								ErrorFlags errFlags(ErrorFlag_ThrowExceptionDisabled);

								NazaraError("Shader code failed to compile (" + stage.GetLog() + ")\n" + code.ToString());
								throw;
							}

							stageIt = shaderStage.cache.emplace(flags, std::move(stage)).first;
						}

						shader->AttachStage(static_cast<ShaderStageType>(i), stageIt->second);
					}
				}

				shader->Link();

				// On construit l'instant
				shaderIt = m_cache.emplace(flags, shader.Get()).first;
			}
			catch (const std::exception&)
			{
				ErrorFlags errFlags(ErrorFlag_ThrowExceptionDisabled);

				NazaraError("Failed to build UberShader instance: " + Error::GetLastError());
				throw;
			}
		}

		return &shaderIt->second;
	}

	void UberShaderPreprocessor::SetShader(ShaderStageType stage, const String& source, const String& shaderFlags, const String& requiredFlags)
	{
		CachedShader& shader = m_shaders[stage];
		shader.present = true;
		shader.source = source;

		// On extrait les flags de la chaîne
		std::vector<String> flags;
		shaderFlags.Split(flags, ' ');

		for (String& flag : flags)
		{
			auto it = m_flags.find(flag);
			if (it == m_flags.end())
				m_flags[flag] = 1U << m_flags.size();

			auto it2 = shader.flags.find(flag);
			if (it2 == shader.flags.end())
				shader.flags[flag] = 1U << shader.flags.size();
		}

		// On construit les flags requis pour l'activation du shader
		shader.requiredFlags = 0;

		flags.clear();
		requiredFlags.Split(flags, ' ');

		for (String& flag : flags)
		{
			UInt32 flagVal;

			auto it = m_flags.find(flag);
			if (it == m_flags.end())
			{
				flagVal = 1U << m_flags.size();
				m_flags[flag] = flagVal;
			}
			else
				flagVal = it->second;

			shader.requiredFlags |= flagVal;
		}
	}

	bool UberShaderPreprocessor::SetShaderFromFile(ShaderStageType stage, const String& filePath, const String& shaderFlags, const String& requiredFlags)
	{
		File file(filePath);
		if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
		{
			NazaraError("Failed to open \"" + filePath + '"');
			return false;
		}

		unsigned int length = static_cast<unsigned int>(file.GetSize());

		String source(length, '\0');

		if (file.Read(&source[0], length) != length)
		{
			NazaraError("Failed to read program file");
			return false;
		}

		file.Close();

		SetShader(stage, source, shaderFlags, requiredFlags);
		return true;
	}

	bool UberShaderPreprocessor::IsSupported()
	{
		return true; // Forcément supporté
	}
}
