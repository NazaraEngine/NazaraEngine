// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderProgram.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/AbstractShaderProgram.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/GLSLProgram.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzShaderProgram::NzShaderProgram() :
m_flags(nzShaderFlags_None),
m_impl(nullptr),
m_compiled(false)
{
}

NzShaderProgram::NzShaderProgram(nzShaderLanguage language) :
m_flags(nzShaderFlags_None),
m_impl(nullptr),
m_compiled(false)
{
	Create(language);
}

NzShaderProgram::NzShaderProgram(NzShaderProgram&& program) :
m_flags(program.m_flags),
m_impl(program.m_impl),
m_compiled(program.m_compiled)
{
	program.m_impl = nullptr;
}

NzShaderProgram::~NzShaderProgram()
{
	Destroy();
}

bool NzShaderProgram::Create(nzShaderLanguage language)
{
	Destroy();

	switch (language)
	{
		case nzShaderLanguage_Cg:
			NazaraError("Cg support is not implemented yet");
			return false;

		case nzShaderLanguage_GLSL:
			m_impl = new NzGLSLProgram(this);
			break;

		default:
			NazaraError("Program language not handled (0x" + NzString::Number(language, 16) + ')');
			return false;
	}

	if (!m_impl->Create())
	{
		NazaraError("Failed to create program");
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	NotifyCreated();
	return true;
}

bool NzShaderProgram::Compile()
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
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

void NzShaderProgram::Destroy()
{
	m_compiled = false;

	if (m_impl)
	{
		NotifyDestroy();

		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
}

NzByteArray NzShaderProgram::GetBinary() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return NzByteArray();
	}

	if (!m_compiled)
	{
		NazaraError("Program is not compiled");
		return NzByteArray();
	}

	if (!m_impl->IsBinaryRetrievable())
	{
		NazaraError("Program binary is not retrievable");
		return NzByteArray();
	}
	#endif

	NzByteArray binary(m_impl->GetBinary());
	if (binary.IsEmpty())
		return NzByteArray();

	NzByteArray byteArray;

	///TODO: ByteStream

	nzUInt32 language = static_cast<nzUInt32>(m_impl->GetLanguage());
	byteArray.Append(&language, sizeof(nzUInt32));
	byteArray.Append(binary);

	return byteArray;
}

nzUInt32 NzShaderProgram::GetFlags() const
{
	return m_flags;
}

NzString NzShaderProgram::GetLog() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NzString error = "Program not created";
		NazaraError(error);

		return error;
	}
	#endif

	return m_impl->GetLog();
}

nzShaderLanguage NzShaderProgram::GetLanguage() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NzString error = "Program not created";
		NazaraError(error);

		return nzShaderLanguage_Unknown;
	}
	#endif

	return m_impl->GetLanguage();
}

NzString NzShaderProgram::GetSourceCode(nzShaderType type) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NzString error = "Program not created";
		NazaraError(error);

		return error;
	}

	if (!IsShaderTypeSupported(type))
	{
		NzString error = "Program type not supported";
		NazaraError(error);

		return error;
	}

	if (!m_impl->IsLoaded(type))
	{
		NzString error = "Program not loaded";
		NazaraError(error);

		return error;
	}
	#endif

	return m_impl->GetSourceCode(type);
}

int NzShaderProgram::GetUniformLocation(const NzString& name) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	return m_impl->GetUniformLocation(name);
}

int NzShaderProgram::GetUniformLocation(nzShaderUniform uniform) const
{
	#ifdef NAZARA_DEBUG
	if (uniform > nzShaderUniform_Max)
	{
		NazaraError("Program uniform out of enum");
		return -1;
	}
	#endif

	return m_impl->GetUniformLocation(uniform);
}

bool NzShaderProgram::HasUniform(const NzString& name) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	return m_impl->GetUniformLocation(name) != -1;
}

bool NzShaderProgram::IsBinaryRetrievable() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	return m_impl->IsBinaryRetrievable();
}

bool NzShaderProgram::IsCompiled() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	return m_compiled;
}

bool NzShaderProgram::IsLoaded(nzShaderType type) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}

	if (!IsShaderTypeSupported(type))
	{
		NazaraError("Program type not supported");
		return false;
	}
	#endif

	return m_impl->IsLoaded(type);
}

bool NzShaderProgram::IsValid() const
{
	return m_impl != nullptr;
}

bool NzShaderProgram::LoadFromBinary(const void* buffer, unsigned int size)
{
	#if NAZARA_RENDERER_SAFE
	if (size <= sizeof(nzUInt32))
	{
		NazaraError("Invalid binary");
		return false;
	}
	#endif

	///TODO: ByteStream
	const nzUInt8* ptr = static_cast<const nzUInt8*>(buffer);

	nzShaderLanguage language = static_cast<nzShaderLanguage>(*reinterpret_cast<const nzUInt32*>(&ptr[0]));
	ptr += sizeof(nzUInt32);

	if (!Create(language))
	{
		NazaraError("Failed to create shader");
		return false;
	}

	if (m_impl->LoadFromBinary(ptr, size-sizeof(nzUInt32)))
	{
		m_compiled = true;

		return true;
	}
	else
		return false;
}

bool NzShaderProgram::LoadFromBinary(const NzByteArray& byteArray)
{
	return LoadFromBinary(byteArray.GetConstBuffer(), byteArray.GetSize());
}

bool NzShaderProgram::LoadShader(nzShaderType type, const NzString& source)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}

	if (!IsShaderTypeSupported(type))
	{
		NazaraError("Program type not supported");
		return false;
	}

	if (source.IsEmpty())
	{
		NazaraError("Empty source code");
		return false;
	}

	if (m_impl->IsLoaded(type))
	{
		NazaraError("Program already loaded");
		return false;
	}
	#endif

	return m_impl->LoadShader(type, source);
}

bool NzShaderProgram::LoadShaderFromFile(nzShaderType type, const NzString& filePath)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}

	if (!IsShaderTypeSupported(type))
	{
		NazaraError("Program type not supported");
		return false;
	}

	if (m_impl->IsLoaded(type))
	{
		NazaraError("Program already loaded");
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
		NazaraError("Failed to read program file");
		return false;
	}

	file.Close();

	return m_impl->LoadShader(type, source);
}

bool NzShaderProgram::SendBoolean(int location, bool value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendBoolean(location, value);
}

bool NzShaderProgram::SendColor(int location, const NzColor& color) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendColor(location, color);
}

bool NzShaderProgram::SendDouble(int location, double value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendDouble(location, value);
}

bool NzShaderProgram::SendFloat(int location, float value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendFloat(location, value);
}

bool NzShaderProgram::SendInteger(int location, int value) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendInteger(location, value);
}

bool NzShaderProgram::SendMatrix(int location, const NzMatrix4d& matrix) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendMatrix(location, matrix);
}

bool NzShaderProgram::SendMatrix(int location, const NzMatrix4f& matrix) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendMatrix(location, matrix);
}

bool NzShaderProgram::SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendTexture(location, texture, textureUnit);
}

bool NzShaderProgram::SendVector(int location, const NzVector2d& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendVector(location, vector);
}

bool NzShaderProgram::SendVector(int location, const NzVector2f& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendVector(location, vector);
}

bool NzShaderProgram::SendVector(int location, const NzVector3d& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendVector(location, vector);
}

bool NzShaderProgram::SendVector(int location, const NzVector3f& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendVector(location, vector);
}

bool NzShaderProgram::SendVector(int location, const NzVector4d& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!NzRenderer::HasCapability(nzRendererCap_FP64))
	{
		NazaraError("FP64 is not supported");
		return false;
	}

	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendVector(location, vector);
}

bool NzShaderProgram::SendVector(int location, const NzVector4f& vector) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("Program not created");
		return false;
	}
	#endif

	if (location == -1)
		return false;

	return m_impl->SendVector(location, vector);
}

void NzShaderProgram::SetFlags(nzUInt32 flags)
{
	m_flags = flags;
}

NzShaderProgram& NzShaderProgram::operator=(NzShaderProgram&& program)
{
	Destroy();

	m_impl = program.m_impl;
	program.m_impl = nullptr;

	return *this;
}

bool NzShaderProgram::IsLanguageSupported(nzShaderLanguage language)
{
	switch (language)
	{
		case nzShaderLanguage_Cg:
			return false; //FIXME: ??

		case nzShaderLanguage_GLSL:
			return true;

		default:
			NazaraError("Program language not handled (0x" + NzString::Number(language, 16) + ')');
			return false;
	}
}

bool NzShaderProgram::IsShaderTypeSupported(nzShaderType type)
{
	switch (type)
	{
		case nzShaderType_Fragment:
		case nzShaderType_Vertex:
			return true;

		case nzShaderType_Geometry:
			return NzOpenGL::GetVersion() >= 320;

		default:
			NazaraError("Program type not handled (0x" + NzString::Number(type, 16) + ')');
			return false;
	}
}
