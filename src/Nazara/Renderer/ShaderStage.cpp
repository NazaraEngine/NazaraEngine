// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	ShaderStage::ShaderStage() :
	m_compiled(false),
	m_id(0)
	{
	}

	ShaderStage::ShaderStage(ShaderStageType stage) :
	ShaderStage()
	{
		Create(stage);
	}

	ShaderStage::ShaderStage(ShaderStage&& stage) :
	m_stage(stage.m_stage),
	m_compiled(stage.m_compiled),
	m_id(stage.m_id)
	{
		stage.m_id = 0;
	}

	ShaderStage::~ShaderStage()
	{
		Destroy();
	}

	bool ShaderStage::Compile()
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_id)
		{
			NazaraError("Shader stage is not initialized");
			return false;
		}
		#endif

		glCompileShader(m_id);

		GLint success;
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

		m_compiled = (success == GL_TRUE);
		if (m_compiled)
			return true;
		else
		{
			NazaraError("Failed to compile shader stage: " + GetLog());
			return false;
		}
	}

	bool ShaderStage::Create(ShaderStageType stage)
	{
		Destroy();

		m_id = glCreateShader(OpenGL::ShaderStage[stage]);
		m_stage = stage;

		return (m_id != 0);
	}

	void ShaderStage::Destroy()
	{
		m_compiled = false;
		if (m_id)
		{
			glDeleteShader(m_id);
			m_id = 0;
		}
	}

	String ShaderStage::GetLog() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_id)
		{
			NazaraError("Shader stage is not initialized");
			return String();
		}
		#endif

		String log;

		GLint length = 0;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
		if (length > 1) // Le caractère de fin faisant partie du compte
		{
			log.Set(length - 1, '\0'); // La taille retournée est celle du buffer (Avec caractère de fin)
			glGetShaderInfoLog(m_id, length, nullptr, &log[0]);
		}
		else
			log = "No log.";

		return log;
	}

	String ShaderStage::GetSource() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_id)
		{
			NazaraError("Shader stage is not initialized");
			return String();
		}
		#endif

		String source;

		GLint length = 0;
		glGetShaderiv(m_id, GL_SHADER_SOURCE_LENGTH, &length);
		if (length > 1) // Le caractère de fin compte
		{
			source.Set(length - 1, '\0'); // La taille retournée est celle du buffer (Avec caractère de fin)
			glGetShaderSource(m_id, length, nullptr, &source[0]);
		}

		return source;
	}

	bool ShaderStage::IsCompiled() const
	{
		return m_compiled;
	}

	bool ShaderStage::IsValid() const
	{
		return m_id != 0;
	}

	void ShaderStage::SetSource(const char* source, unsigned int length)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_id)
		{
			NazaraError("Shader stage is not initialized");
			return;
		}
		#endif

		glShaderSource(m_id, 1, &source, reinterpret_cast<const GLint*>(&length));
	}

	void ShaderStage::SetSource(const String& source)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_id)
		{
			NazaraError("Shader stage is not initialized");
			return;
		}
		#endif

		const char* tmp = source.GetConstBuffer();
		GLint length = source.GetSize();
		glShaderSource(m_id, 1, &tmp, &length);
	}

	bool ShaderStage::SetSourceFromFile(const String& filePath)
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_id)
		{
			NazaraError("Shader stage is not initialized");
			return false;
		}
		#endif

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

		SetSource(source);
		return true;
	}

	ShaderStage& ShaderStage::operator=(ShaderStage&& shader)
	{
		Destroy();

		m_compiled = shader.m_compiled;
		m_id = shader.m_id;
		m_stage = shader.m_stage;

		shader.m_id = 0;

		return *this;
	}

	// Fonctions OpenGL
	unsigned int ShaderStage::GetOpenGLID() const
	{
		return m_id;
	}

	bool ShaderStage::IsSupported(ShaderStageType stage)
	{
		switch (stage)
		{
			case ShaderStageType_Fragment:
			case ShaderStageType_Geometry:
			case ShaderStageType_Vertex:
				return true;

			default:
				NazaraError("Shader stage not handled (0x" + String::Number(stage, 16) + ')');
				return false;
			}
	}
}
