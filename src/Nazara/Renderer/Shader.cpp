// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzShader::NzShader() :
m_linked(false),
m_program(0)
{
}

NzShader::NzShader(NzShader&& shader) :
m_linked(shader.m_linked),
m_program(shader.m_program)
{
	for (unsigned int i = 0; i <= nzShaderStage_Max; ++i)
		m_attachedShaders[i] = std::move(shader.m_attachedShaders[i]);

	shader.m_linked = false;
	shader.m_program = 0;
}

NzShader::~NzShader()
{
	Destroy();
}

void NzShader::AttachStage(nzShaderStage stage, const NzShaderStage& shaderStage)
{
	#if NAZARA_RENDERER_SAFE
	if (!m_program)
	{
		NazaraError("Invalid program");
		return;
	}

	if (!shaderStage.IsValid())
	{
		NazaraError("Invalid shader stage");
		return;
	}

	if (!shaderStage.IsCompiled())
	{
		NazaraError("Shader stage must be compiled");
		return;
	}
	#endif

	unsigned int shader = shaderStage.GetOpenGLID();

	#if NAZARA_RENDERER_SAFE
	if (std::find(m_attachedShaders[stage].begin(), m_attachedShaders[stage].end(), shader) != m_attachedShaders[stage].end())
	{
		NazaraError("Shader stage is already attached");
		return;
	}
	#endif

	glAttachShader(m_program, shader);
	m_attachedShaders[stage].push_back(shader);
}

bool NzShader::AttachStageFromFile(nzShaderStage stage, const NzString& filePath)
{
	NzShaderStage shaderStage(stage);
	if (!shaderStage.IsValid())
	{
		NazaraError("Failed to create shader stage");
		return false;
	}

	shaderStage.SetSourceFromFile(filePath);

	if (!shaderStage.Compile())
	{
		NazaraError("Failed to compile stage: " + shaderStage.GetLog());
		return false;
	}

	AttachStage(stage, shaderStage);
	return true;
}

bool NzShader::AttachStageFromSource(nzShaderStage stage, const char* source, unsigned int length)
{
	NzShaderStage shaderStage(stage);
	if (!shaderStage.IsValid())
	{
		NazaraError("Failed to create shader stage");
		return false;
	}

	shaderStage.SetSource(source, length);

	if (!shaderStage.Compile())
	{
		NazaraError("Failed to compile stage: " + shaderStage.GetLog());
		return false;
	}

	AttachStage(stage, shaderStage);
	return true;
}

bool NzShader::AttachStageFromSource(nzShaderStage stage, const NzString& source)
{
	NzShaderStage shaderStage(stage);
	if (!shaderStage.IsValid())
	{
		NazaraError("Failed to create shader stage");
		return false;
	}

	shaderStage.SetSource(source);

	if (!shaderStage.Compile())
	{
		NazaraError("Failed to compile stage: " + shaderStage.GetLog());
		return false;
	}

	AttachStage(stage, shaderStage);
	return true;
}

void NzShader::Bind() const
{
	NzOpenGL::BindProgram(m_program);
}

bool NzShader::Create()
{
	NzContext::EnsureContext();

	m_program = glCreateProgram();
	if (!m_program)
	{
		NazaraError("Failed to create program");
		return false;
	}

	m_linked = false;

	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_InstanceData0], "InstanceData0");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_InstanceData1], "InstanceData1");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_InstanceData2], "InstanceData2");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_InstanceData3], "InstanceData3");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_InstanceData4], "InstanceData4");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_InstanceData5], "InstanceData5");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Color],         "VertexColor");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Normal],        "VertexNormal");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Position],      "VertexPosition");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Tangent],       "VertexTangent");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_TexCoord],      "VertexTexCoord");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Userdata0],     "VertexUserdata0");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Userdata1],     "VertexUserdata1");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Userdata2],     "VertexUserdata2");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Userdata3],     "VertexUserdata3");
	glBindAttribLocation(m_program, NzOpenGL::VertexComponentIndex[nzVertexComponent_Userdata4],     "VertexUserdata4");

	if (NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets))
	{
		NzString uniform;
		uniform = "RenderTarget";

		unsigned int maxRenderTargets = NzRenderer::GetMaxRenderTargets();
		for (unsigned int i = 0; i < maxRenderTargets; ++i)
		{
			NzString uniformName = uniform + NzString::Number(i);
			glBindFragDataLocation(m_program, i, uniformName.GetConstBuffer());
		}
	}

	if (NzOpenGL::IsSupported(nzOpenGLExtension_GetProgramBinary))
		glProgramParameteri(m_program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

	return true;
}

void NzShader::Destroy()
{
	NzContext::EnsureContext();

	NzOpenGL::DeleteProgram(m_program);
}

NzByteArray NzShader::GetBinary() const
{
	NzByteArray byteArray;

	NzContext::EnsureContext();

	GLint binaryLength = 0;
	glGetProgramiv(m_program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

	if (binaryLength > 0)
	{
		byteArray.Resize(sizeof(nzUInt64) + binaryLength);

		nzUInt8* buffer = byteArray.GetBuffer();

		GLenum binaryFormat;
		glGetProgramBinary(m_program, binaryLength, nullptr, &binaryFormat, &buffer[sizeof(nzUInt64)]);

		// On stocke le format au début du binaire
		*reinterpret_cast<nzUInt64*>(&buffer[0]) = binaryFormat;
	}

	return byteArray;
}

NzString NzShader::GetLog() const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_program)
	{
		NazaraError("Shader is not initialized");
		return NzString();
	}
	#endif

	NzString log;

	GLint length = 0;
	glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) // Le caractère de fin faisant partie du compte
	{
		log.Set(length - 1, '\0'); // La taille retournée est celle du buffer (Avec caractère de fin)
		glGetProgramInfoLog(m_program, length, nullptr, &log[0]);
	}
	else
		log = "No log.";

	return log;
}

NzString NzShader::GetSourceCode(nzShaderStage stage) const
{
	if (!HasStage(stage))
		return NzString();

	NzContext::EnsureContext();

	static const char sep[] = "\n////////////////////////////////////////////////////////////////////////////////\n\n";

	unsigned int totalLength = 0;
	for (unsigned int shader : m_attachedShaders[stage])
	{
        GLint length;
        glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

        totalLength += length - 1;
	}

	totalLength += (m_attachedShaders[stage].size()-1)*(sizeof(sep)/sizeof(char));

	NzString source(totalLength, '\0');

	unsigned int offset = 0;
	for (unsigned int shader : m_attachedShaders[stage])
	{
		if (offset > 0)
		{
			std::memcpy(&source[offset], sep, sizeof(sep)/sizeof(char));
			offset += sizeof(sep)/sizeof(char);
		}

		GLint length;
		glGetShaderSource(shader, totalLength, &length, &source[offset]);

		offset += length;
	}

	return source;
}

int NzShader::GetUniformLocation(const NzString& name) const
{
	NzContext::EnsureContext();

	return glGetUniformLocation(m_program, name.GetConstBuffer());
}

int NzShader::GetUniformLocation(nzShaderUniform shaderUniform) const
{
	return m_uniformLocations[shaderUniform];
}

bool NzShader::HasStage(nzShaderStage stage) const
{
	return !m_attachedShaders[stage].empty();
}

bool NzShader::IsBinaryRetrievable() const
{
	return NzOpenGL::IsSupported(nzOpenGLExtension_GetProgramBinary);
}

bool NzShader::IsLinked() const
{
	return m_linked;
}

bool NzShader::IsValid() const
{
	return m_program != 0;
}

bool NzShader::Link()
{
	NzContext::EnsureContext();

	glLinkProgram(m_program);

	return PostLinkage();
}

bool NzShader::LoadFromBinary(const void* buffer, unsigned int size)
{
	#if NAZARA_RENDERER_SAFE
	if (!glProgramBinary)
	{
		NazaraError("GL_ARB_get_program_binary not supported");
		return false;
	}

	if (!buffer || size < sizeof(nzUInt64))
	{
		NazaraError("Invalid buffer");
		return false;
	}
	#endif

	NzContext::EnsureContext();

	const nzUInt8* ptr = reinterpret_cast<const nzUInt8*>(buffer);

	// On récupère le format au début du binaire
	///TODO: ByteStream ?
	GLenum binaryFormat = static_cast<GLenum>(*reinterpret_cast<const nzUInt64*>(&ptr[0]));
	ptr += sizeof(nzUInt64);

	glProgramBinary(m_program, binaryFormat, ptr, size - sizeof(nzUInt64));

	return PostLinkage();
}

bool NzShader::LoadFromBinary(const NzByteArray& byteArray)
{
    return LoadFromBinary(byteArray.GetConstBuffer(), byteArray.GetSize());
}

void NzShader::SendBoolean(int location, bool value) const
{
	if (location == -1)
		return;

	if (glProgramUniform1i)
		glProgramUniform1i(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1i(location, value);
	}
}

void NzShader::SendColor(int location, const NzColor& color) const
{
	if (location == -1)
		return;

	NzVector4f vecColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);

	if (glProgramUniform4fv)
		glProgramUniform4fv(m_program, location, 1, vecColor);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4fv(location, 1, vecColor);
	}
}

void NzShader::SendDouble(int location, double value) const
{
	if (location == -1)
		return;

	if (glProgramUniform1d)
		glProgramUniform1d(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1d(location, value);
	}
}

void NzShader::SendDoubleArray(int location, const double* values, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform1dv)
		glProgramUniform1dv(m_program, location, count, values);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1dv(location, count, values);
	}
}

void NzShader::SendFloat(int location, float value) const
{
	if (location == -1)
		return;

	if (glProgramUniform1f)
		glProgramUniform1f(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1f(location, value);
	}
}

void NzShader::SendFloatArray(int location, const float* values, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform1fv)
		glProgramUniform1fv(m_program, location, count, values);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1fv(location, count, values);
	}
}

void NzShader::SendInteger(int location, int value) const
{
	if (location == -1)
		return;

	if (glProgramUniform1i)
		glProgramUniform1i(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1i(location, value);
	}
}

void NzShader::SendIntegerArray(int location, const int* values, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform1iv)
		glProgramUniform1iv(m_program, location, count, values);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1iv(location, count, values);
	}
}

void NzShader::SendMatrix(int location, const NzMatrix4d& matrix) const
{
	if (location == -1)
		return;

	if (glProgramUniformMatrix4dv)
		glProgramUniformMatrix4dv(m_program, location, 1, GL_FALSE, matrix);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniformMatrix4dv(location, 1, GL_FALSE, matrix);
	}
}

void NzShader::SendMatrix(int location, const NzMatrix4f& matrix) const
{
	if (location == -1)
		return;

	if (glProgramUniformMatrix4fv)
		glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, matrix);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
	}
}

void NzShader::SendVector(int location, const NzVector2d& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform2dv)
		glProgramUniform2dv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2dv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector2f& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform2fv)
		glProgramUniform2fv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2fv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector2i& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform2fv)
		glProgramUniform2iv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2iv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector3d& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform3dv)
		glProgramUniform3dv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3dv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector3f& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform3fv)
		glProgramUniform3fv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3fv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector3i& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform3iv)
		glProgramUniform3iv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3iv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector4d& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform4dv)
		glProgramUniform4dv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4dv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector4f& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform4fv)
		glProgramUniform4fv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4fv(location, 1, vector);
	}
}

void NzShader::SendVector(int location, const NzVector4i& vector) const
{
	if (location == -1)
		return;

	if (glProgramUniform4iv)
		glProgramUniform4iv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4iv(location, 1, vector);
	}
}

void NzShader::SendVectorArray(int location, const NzVector2d* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform2dv)
		glProgramUniform2dv(m_program, location, count, reinterpret_cast<const double*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2dv(location, count, reinterpret_cast<const double*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector2f* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform2fv)
		glProgramUniform2fv(m_program, location, count, reinterpret_cast<const float*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2fv(location, count, reinterpret_cast<const float*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector2i* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform2iv)
		glProgramUniform2iv(m_program, location, count, reinterpret_cast<const int*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2iv(location, count, reinterpret_cast<const int*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector3d* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform3dv)
		glProgramUniform3dv(m_program, location, count, reinterpret_cast<const double*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3dv(location, count, reinterpret_cast<const double*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector3f* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform3fv)
		glProgramUniform3fv(m_program, location, count, reinterpret_cast<const float*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3fv(location, count, reinterpret_cast<const float*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector3i* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform3iv)
		glProgramUniform3iv(m_program, location, count, reinterpret_cast<const int*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3iv(location, count, reinterpret_cast<const int*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector4d* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform4dv)
		glProgramUniform4dv(m_program, location, count, reinterpret_cast<const double*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4dv(location, count, reinterpret_cast<const double*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector4f* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform4fv)
		glProgramUniform4fv(m_program, location, count, reinterpret_cast<const float*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4fv(location, count, reinterpret_cast<const float*>(vectors));
	}
}

void NzShader::SendVectorArray(int location, const NzVector4i* vectors, unsigned int count) const
{
	if (location == -1)
		return;

	if (glProgramUniform4iv)
		glProgramUniform4iv(m_program, location, count, reinterpret_cast<const int*>(vectors));
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4iv(location, count, reinterpret_cast<const int*>(vectors));
	}
}

unsigned int NzShader::GetOpenGLID() const
{
	return m_program;
}

NzShader& NzShader::operator=(NzShader&& shader)
{
	Destroy();

	for (unsigned int i = 0; i <= nzShaderStage_Max; ++i)
		m_attachedShaders[i] = std::move(shader.m_attachedShaders[i]);

	m_linked = shader.m_linked;
	m_program = shader.m_program;

	shader.m_linked = false;
	shader.m_program = 0;

	return *this;
}

bool NzShader::IsStageSupported(nzShaderStage stage)
{
	return NzShaderStage::IsSupported(stage);
}

bool NzShader::PostLinkage()
{
	GLint success;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);

	m_linked = (success == GL_TRUE);
	if (m_linked)
	{
		// Pour éviter de se tromper entre le nom et la constante
		#define CacheUniform(name) m_uniformLocations[nzShaderUniform_##name] = glGetUniformLocation(m_program, #name)

		CacheUniform(EyePosition);
		CacheUniform(InvProjMatrix);
		CacheUniform(InvTargetSize);
		CacheUniform(InvViewMatrix);
		CacheUniform(InvViewProjMatrix);
		CacheUniform(InvWorldMatrix);
		CacheUniform(InvWorldViewMatrix);
		CacheUniform(InvWorldViewProjMatrix);
		CacheUniform(ProjMatrix);
		CacheUniform(SceneAmbient);
		CacheUniform(TargetSize);
		CacheUniform(ViewMatrix);
		CacheUniform(ViewProjMatrix);
		CacheUniform(WorldMatrix);
		CacheUniform(WorldViewMatrix);
		CacheUniform(WorldViewProjMatrix);

		#undef CacheUniform

		return true;
	}
	else
	{
		NazaraError("Failed to link shader: " + GetLog());
		return false;
	}
}
