// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzShaderStage::NzShaderStage() :
m_compiled(false),
m_id(0)
{
}

NzShaderStage::NzShaderStage(nzShaderStage stage) :
NzShaderStage()
{
	Create(stage);
}

NzShaderStage::NzShaderStage(NzShaderStage&& stage) :
m_stage(stage.m_stage),
m_compiled(stage.m_compiled),
m_id(stage.m_id)
{
	stage.m_id = 0;
}

NzShaderStage::~NzShaderStage()
{
	Destroy();
}

bool NzShaderStage::Compile()
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

bool NzShaderStage::Create(nzShaderStage stage)
{
	Destroy();

	m_id = glCreateShader(NzOpenGL::ShaderStage[stage]);
	m_stage = stage;

	return (m_id != 0);
}

void NzShaderStage::Destroy()
{
	m_compiled = false;
	if (m_id)
	{
		glDeleteShader(m_id);
		m_id = 0;
	}
}

NzString NzShaderStage::GetLog() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_id)
	{
		NazaraError("Shader stage is not initialized");
		return NzString();
	}
	#endif

	NzString log;

	GLint length = 0;
	glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) // Le caractère de fin faisant partie du compte
	{
		log.Resize(length - 1); // La taille retournée est celle du buffer (Avec caractère de fin)
		glGetShaderInfoLog(m_id, length, nullptr, &log[0]);
	}
	else
		log = "No log.";

	return log;
}

NzString NzShaderStage::GetSource() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_id)
	{
		NazaraError("Shader stage is not initialized");
		return NzString();
	}
	#endif

	NzString source;

	GLint length = 0;
	glGetShaderiv(m_id, GL_SHADER_SOURCE_LENGTH, &length);
	if (length > 1) // Le caractère de fin compte
	{
		source.Resize(length - 1); // La taille retournée est celle du buffer (Avec caractère de fin)
		glGetShaderSource(m_id, length, nullptr, &source[0]);
	}

	return source;
}

bool NzShaderStage::IsCompiled() const
{
	return m_compiled;
}

bool NzShaderStage::IsValid() const
{
	return m_id != 0;
}

void NzShaderStage::SetSource(const char* source, unsigned int length)
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

void NzShaderStage::SetSource(const NzString& source)
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

bool NzShaderStage::SetSourceFromFile(const NzString& filePath)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_id)
	{
		NazaraError("Shader stage is not initialized");
		return false;
	}
	#endif

	NzFile file(filePath);
	if (!file.Open(NzFile::ReadOnly | NzFile::Text))
	{
		NazaraError("Failed to open \"" + filePath + '"');
		return false;
	}

	unsigned int length = static_cast<unsigned int>(file.GetSize());

	NzString source;
	source.Resize(length);

	if (file.Read(&source[0], length) != length)
	{
		NazaraError("Failed to read program file");
		return false;
	}

	file.Close();

	SetSource(source);
	return true;
}

NzShaderStage& NzShaderStage::operator=(NzShaderStage&& shader)
{
	Destroy();

	m_compiled = shader.m_compiled;
	m_id = shader.m_id;
	m_stage = shader.m_stage;

	shader.m_id = 0;

	return *this;
}

// Fonctions OpenGL
unsigned int NzShaderStage::GetOpenGLID() const
{
	return m_id;
}

bool NzShaderStage::IsSupported(nzShaderStage stage)
{
	switch (stage)
	{
		case nzShaderStage_Fragment:
		case nzShaderStage_Vertex:
			return true;

		case nzShaderStage_Geometry:
			return NzOpenGL::GetVersion() >= 320;

		default:
			NazaraError("Shader stage not handled (0x" + NzString::Number(stage, 16) + ')');
			return false;
	}
}
