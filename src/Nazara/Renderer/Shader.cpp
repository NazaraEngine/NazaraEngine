// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/GLSLShader.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderImpl.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzShader::NzShader(nzShaderLanguage language)
{
	Create(language);
}

NzShader::NzShader(NzShader&& shader) :
m_impl(shader.m_impl)
{
	shader.m_impl = nullptr;
}

NzShader::~NzShader()
{
	Destroy();
}

bool NzShader::Create(nzShaderLanguage language)
{
	Destroy();

	switch (language)
	{
		case nzShaderLanguage_Cg:
			NazaraError("Cg support is not implemented yet");
			return false;

		case nzShaderLanguage_GLSL:
			m_impl = new NzGLSLShader(this);
			break;

		default:
			NazaraError("Shader language not handled (0x" + NzString::Number(language, 16) + ')');
			return false;
	}

	if (!m_impl->Create())
	{
		NazaraError("Failed to create shader");
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	NotifyCreated();
	return true;
}

bool NzShader::Compile()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	if (m_impl->Compile())
	{
		m_compiled = true;
		return true;
	}
	else
		return false;
}

void NzShader::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
}

nzUInt32 NzShader::GetFlags() const
{
	return m_flags;
}

NzString NzShader::GetLog() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NzString error = "Shader not created";
		NazaraError(error);

		return error;
	}
	#endif

	return m_impl->GetLog();
}

nzShaderLanguage NzShader::GetLanguage() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NzString error = "Shader not created";
		NazaraError(error);

		return nzShaderLanguage_Unknown;
	}
	#endif

	return m_impl->GetLanguage();
}

NzString NzShader::GetSourceCode(nzShaderType type) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NzString error = "Shader not created";
		NazaraError(error);

		return error;
	}

	if (!IsTypeSupported(type))
	{
		NzString error = "Shader type not supported";
		NazaraError(error);

		return error;
	}

	if (!m_impl->IsLoaded(type))
	{
		NzString error = "Shader not loaded";
		NazaraError(error);

		return error;
	}
	#endif

	return m_impl->GetSourceCode(type);
}

int NzShader::GetUniformLocation(const NzString& name) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->GetUniformLocation(name);
}

bool NzShader::HasUniform(const NzString& name) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->GetUniformLocation(name) != -1;
}

bool NzShader::IsCompiled() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_compiled;
}

bool NzShader::IsLoaded(nzShaderType type) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!IsTypeSupported(type))
	{
		NazaraError("Shader type not supported");
		return false;
	}
	#endif

	return m_impl->IsLoaded(type);
}

bool NzShader::IsValid() const
{
	return m_impl != nullptr;
}

bool NzShader::Load(nzShaderType type, const NzString& source)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!IsTypeSupported(type))
	{
		NazaraError("Shader type not supported");
		return false;
	}

	if (source.IsEmpty())
	{
		NazaraError("Empty source code");
		return false;
	}

	if (m_impl->IsLoaded(type))
	{
		NazaraError("Shader already loaded");
		return false;
	}
	#endif

	return m_impl->Load(type, source);
}

bool NzShader::LoadFromFile(nzShaderType type, const NzString& filePath)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!IsTypeSupported(type))
	{
		NazaraError("Shader type not supported");
		return false;
	}

	if (m_impl->IsLoaded(type))
	{
		NazaraError("Shader already loaded");
		return false;
	}
	#endif

	NzFile file(filePath);
	if (!file.Open(NzFile::ReadOnly | NzFile::Text))
	{
		NazaraError("Failed to open \"" + filePath + '"');
		return false;
	}

	unsigned int length = file.GetSize();

	NzString source;
	source.Resize(length);

	if (file.Read(&source[0], length) != length)
	{
		NazaraError("Failed to read shader file");
		return false;
	}

	file.Close();

	return m_impl->Load(type, source);
}

bool NzShader::Lock()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->Lock();
}

bool NzShader::SendBoolean(int location, bool value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendBoolean(location, value);
}

bool NzShader::SendColor(int location, const NzColor& color) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendColor(location, color);
}

bool NzShader::SendDouble(int location, double value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendDouble(location, value);
}

bool NzShader::SendFloat(int location, float value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendFloat(location, value);
}

bool NzShader::SendInteger(int location, int value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendInteger(location, value);
}

bool NzShader::SendMatrix(int location, const NzMatrix4d& matrix) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendMatrix(location, matrix);
}

bool NzShader::SendMatrix(int location, const NzMatrix4f& matrix) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendMatrix(location, matrix);
}

bool NzShader::SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendTexture(location, texture, textureUnit);
}

bool NzShader::SendVector(int location, const NzVector2d& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendVector(location, vector);
}

bool NzShader::SendVector(int location, const NzVector2f& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendVector(location, vector);
}

bool NzShader::SendVector(int location, const NzVector3d& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendVector(location, vector);
}

bool NzShader::SendVector(int location, const NzVector3f& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendVector(location, vector);
}

bool NzShader::SendVector(int location, const NzVector4d& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendVector(location, vector);
}

bool NzShader::SendVector(int location, const NzVector4f& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (location == -1)
	{
		NazaraError("Invalid location");
		return false;
	}
	#endif

	return m_impl->SendVector(location, vector);
}

void NzShader::SetFlags(nzUInt32 flags)
{
	m_flags = flags;
}

void NzShader::Unlock()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return;
	}
	#endif

	return m_impl->Unlock();
}

NzShader& NzShader::operator=(NzShader&& shader)
{
	Destroy();

	m_impl = shader.m_impl;
	shader.m_impl = nullptr;

	return *this;
}

bool NzShader::IsLanguageSupported(nzShaderLanguage language)
{
	switch (language)
	{
		case nzShaderLanguage_Cg:
			return false; //FIXME: ??

		case nzShaderLanguage_GLSL:
			return true;

		default:
			NazaraError("Shader language not handled (0x" + NzString::Number(language, 16) + ')');
			return false;
	}
}

bool NzShader::IsTypeSupported(nzShaderType type)
{
	switch (type)
	{
		case nzShaderType_Fragment:
		case nzShaderType_Vertex:
			return true;

		case nzShaderType_Geometry:
			return false; // ??

		default:
			NazaraError("Shader type not handled (0x" + NzString::Number(type, 16) + ')');
			return false;
	}
}
