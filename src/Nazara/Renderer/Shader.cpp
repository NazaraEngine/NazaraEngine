// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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

NzShader::NzShader() :
m_impl(nullptr),
m_compiled(false)
{
}

NzShader::NzShader(nzShaderLanguage language) :
m_impl(nullptr),
m_compiled(false)
{
	Create(language);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create shader");
		throw std::runtime_error("Constructor failed");
	}
	#endif
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
		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
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

bool NzShader::SendBoolean(const NzString& name, bool value)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendBoolean(name, value);
}

bool NzShader::SendDouble(const NzString& name, double value)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!NazaraRenderer->HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}
	#endif

	return m_impl->SendDouble(name, value);
}

bool NzShader::SendFloat(const NzString& name, float value)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendFloat(name, value);
}

bool NzShader::SendInteger(const NzString& name, int value)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendInteger(name, value);
}

bool NzShader::SendMatrix(const NzString& name, const NzMatrix4d& matrix)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!NazaraRenderer->HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}
	#endif

	return m_impl->SendMatrix(name, matrix);
}

bool NzShader::SendMatrix(const NzString& name, const NzMatrix4f& matrix)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendMatrix(name, matrix);
}

bool NzShader::SendVector(const NzString& name, const NzVector2d& vector)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!NazaraRenderer->HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}
	#endif

	return m_impl->SendVector(name, vector);
}

bool NzShader::SendVector(const NzString& name, const NzVector2f& vector)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendVector(name, vector);
}

bool NzShader::SendVector(const NzString& name, const NzVector3d& vector)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!NazaraRenderer->HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}
	#endif

	return m_impl->SendVector(name, vector);
}

bool NzShader::SendVector(const NzString& name, const NzVector3f& vector)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendVector(name, vector);
}

bool NzShader::SendVector(const NzString& name, const NzVector4d& vector)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}

	if (!NazaraRenderer->HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}
	#endif

	return m_impl->SendVector(name, vector);
}

bool NzShader::SendVector(const NzString& name, const NzVector4f& vector)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendVector(name, vector);
}

bool NzShader::SendTexture(const NzString& name, NzTexture* texture)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Shader not created");
		return false;
	}
	#endif

	return m_impl->SendTexture(name, texture);
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

bool NzShader::IsLanguageSupported(nzShaderLanguage language)
{
	switch (language)
	{
		case nzShaderLanguage_Cg:
			return false; // ??

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
