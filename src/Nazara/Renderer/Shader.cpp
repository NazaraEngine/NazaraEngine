// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	Shader::Shader() :
	m_linked(false),
	m_program(0)
	{
	}

	Shader::~Shader()
	{
		OnShaderRelease(this);

		Destroy();
	}

	void Shader::AttachStage(ShaderStageType stage, const ShaderStage& shaderStage)
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

	bool Shader::AttachStageFromFile(ShaderStageType stage, const String& filePath)
	{
		ShaderStage shaderStage(stage);
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

	bool Shader::AttachStageFromSource(ShaderStageType stage, const char* source, unsigned int length)
	{
		ShaderStage shaderStage(stage);
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

	bool Shader::AttachStageFromSource(ShaderStageType stage, const String& source)
	{
		ShaderStage shaderStage(stage);
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

	void Shader::Bind() const
	{
		OpenGL::BindProgram(m_program);
	}

	bool Shader::Create()
	{
		Context::EnsureContext();

		m_program = glCreateProgram();
		if (!m_program)
		{
			NazaraError("Failed to create program");
			return false;
		}

		m_linked = false;

		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_InstanceData0], "InstanceData0");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_InstanceData1], "InstanceData1");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_InstanceData2], "InstanceData2");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_InstanceData3], "InstanceData3");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_InstanceData4], "InstanceData4");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_InstanceData5], "InstanceData5");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Color],         "VertexColor");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Normal],        "VertexNormal");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Position],      "VertexPosition");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Tangent],       "VertexTangent");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_TexCoord],      "VertexTexCoord");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Userdata0],     "VertexUserdata0");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Userdata1],     "VertexUserdata1");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Userdata2],     "VertexUserdata2");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Userdata3],     "VertexUserdata3");
		glBindAttribLocation(m_program, OpenGL::VertexComponentIndex[VertexComponent_Userdata4],     "VertexUserdata4");

		String uniform;
		uniform = "RenderTarget";

		unsigned int maxRenderTargets = Renderer::GetMaxRenderTargets();
		for (unsigned int i = 0; i < maxRenderTargets; ++i)
		{
			String uniformName = uniform + String::Number(i);
			glBindFragDataLocation(m_program, i, uniformName.GetConstBuffer());
		}

		if (OpenGL::IsSupported(OpenGLExtension_GetProgramBinary))
			glProgramParameteri(m_program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

		return true;
	}

	void Shader::Destroy()
	{
		if (m_program)
		{
			OnShaderDestroy(this);

			Context::EnsureContext();
			OpenGL::DeleteProgram(m_program);
			m_program = 0;
		}
	}

	ByteArray Shader::GetBinary() const
	{
		ByteArray byteArray;

		Context::EnsureContext();

		GLint binaryLength = 0;
		glGetProgramiv(m_program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

		if (binaryLength > 0)
		{
			byteArray.Reserve(sizeof(UInt64) + binaryLength);

			UInt8* buffer = byteArray.GetBuffer();

			GLenum binaryFormat;
			glGetProgramBinary(m_program, binaryLength, nullptr, &binaryFormat, &buffer[sizeof(UInt64)]);

			// On stocke le format au début du binaire
			*reinterpret_cast<UInt64*>(&buffer[0]) = binaryFormat;
		}

		return byteArray;
	}

	String Shader::GetLog() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_program)
		{
			NazaraError("Shader is not initialized");
			return String();
		}
		#endif

		String log;

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

	String Shader::GetSourceCode(ShaderStageType stage) const
	{
		if (!HasStage(stage))
			return String();

		Context::EnsureContext();

		static const char sep[] = "\n////////////////////////////////////////////////////////////////////////////////\n\n";

		unsigned int totalLength = 0;
		for (unsigned int shader : m_attachedShaders[stage])
		{
			GLint length;
			glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

			totalLength += length - 1;
		}

		totalLength += (m_attachedShaders[stage].size()-1) * (sizeof(sep)/sizeof(char));

		String source(totalLength, '\0');

		unsigned int offset = 0;
		for (unsigned int shader : m_attachedShaders[stage])
		{
			if (offset > 0)
			{
				std::memcpy(&source[offset], sep, sizeof(sep));
				offset += sizeof(sep)/sizeof(char);
			}

			GLint length;
			glGetShaderSource(shader, totalLength, &length, &source[offset]);

			offset += length;
		}

		return source;
	}

	int Shader::GetUniformLocation(const String& name) const
	{
		Context::EnsureContext();

		return glGetUniformLocation(m_program, name.GetConstBuffer());
	}

	int Shader::GetUniformLocation(ShaderUniform shaderUniform) const
	{
		return m_uniformLocations[shaderUniform];
	}

	bool Shader::HasStage(ShaderStageType stage) const
	{
		return !m_attachedShaders[stage].empty();
	}

	bool Shader::IsBinaryRetrievable() const
	{
		return OpenGL::IsSupported(OpenGLExtension_GetProgramBinary);
	}

	bool Shader::IsLinked() const
	{
		return m_linked;
	}

	bool Shader::IsValid() const
	{
		return m_program != 0;
	}

	bool Shader::Link()
	{
		Context::EnsureContext();

		glLinkProgram(m_program);

		return PostLinkage();
	}

	bool Shader::LoadFromBinary(const void* buffer, unsigned int size)
	{
		#if NAZARA_RENDERER_SAFE
		if (!glProgramBinary)
		{
			NazaraError("GL_ARB_get_program_binary not supported");
			return false;
		}

		if (!buffer || size < sizeof(UInt64))
		{
			NazaraError("Invalid buffer");
			return false;
		}
		#endif

		Context::EnsureContext();

		const UInt8* ptr = reinterpret_cast<const UInt8*>(buffer);

		// On récupère le format au début du binaire
		///TODO: ByteStream ?
		GLenum binaryFormat = static_cast<GLenum>(*reinterpret_cast<const UInt64*>(&ptr[0]));
		ptr += sizeof(UInt64);

		glProgramBinary(m_program, binaryFormat, ptr, size - sizeof(UInt64));

		return PostLinkage();
	}

	bool Shader::LoadFromBinary(const ByteArray& byteArray)
	{
		return LoadFromBinary(byteArray.GetConstBuffer(), byteArray.GetSize());
	}

	void Shader::SendBoolean(int location, bool value) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1i)
			glProgramUniform1i(m_program, location, value);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1i(location, value);
		}
	}

	void Shader::SendColor(int location, const Color& color) const
	{
		if (location == -1)
			return;

		Vector4f vecColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);

		if (glProgramUniform4fv)
			glProgramUniform4fv(m_program, location, 1, vecColor);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4fv(location, 1, vecColor);
		}
	}

	void Shader::SendDouble(int location, double value) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1d)
			glProgramUniform1d(m_program, location, value);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1d(location, value);
		}
	}

	void Shader::SendDoubleArray(int location, const double* values, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1dv)
			glProgramUniform1dv(m_program, location, count, values);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1dv(location, count, values);
		}
	}

	void Shader::SendFloat(int location, float value) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1f)
			glProgramUniform1f(m_program, location, value);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1f(location, value);
		}
	}

	void Shader::SendFloatArray(int location, const float* values, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1fv)
			glProgramUniform1fv(m_program, location, count, values);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1fv(location, count, values);
		}
	}

	void Shader::SendInteger(int location, int value) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1i)
			glProgramUniform1i(m_program, location, value);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1i(location, value);
		}
	}

	void Shader::SendIntegerArray(int location, const int* values, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform1iv)
			glProgramUniform1iv(m_program, location, count, values);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform1iv(location, count, values);
		}
	}

	void Shader::SendMatrix(int location, const Matrix4d& matrix) const
	{
		if (location == -1)
			return;

		if (glProgramUniformMatrix4dv)
			glProgramUniformMatrix4dv(m_program, location, 1, GL_FALSE, matrix);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniformMatrix4dv(location, 1, GL_FALSE, matrix);
		}
	}

	void Shader::SendMatrix(int location, const Matrix4f& matrix) const
	{
		if (location == -1)
			return;

		if (glProgramUniformMatrix4fv)
			glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, matrix);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
		}
	}

	void Shader::SendVector(int location, const Vector2d& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform2dv)
			glProgramUniform2dv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform2dv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector2f& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform2fv)
			glProgramUniform2fv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform2fv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector2i& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform2fv)
			glProgramUniform2iv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform2iv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector3d& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform3dv)
			glProgramUniform3dv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform3dv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector3f& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform3fv)
			glProgramUniform3fv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform3fv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector3i& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform3iv)
			glProgramUniform3iv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform3iv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector4d& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform4dv)
			glProgramUniform4dv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4dv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector4f& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform4fv)
			glProgramUniform4fv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4fv(location, 1, vector);
		}
	}

	void Shader::SendVector(int location, const Vector4i& vector) const
	{
		if (location == -1)
			return;

		if (glProgramUniform4iv)
			glProgramUniform4iv(m_program, location, 1, vector);
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4iv(location, 1, vector);
		}
	}

	void Shader::SendVectorArray(int location, const Vector2d* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform2dv)
			glProgramUniform2dv(m_program, location, count, reinterpret_cast<const double*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform2dv(location, count, reinterpret_cast<const double*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector2f* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform2fv)
			glProgramUniform2fv(m_program, location, count, reinterpret_cast<const float*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform2fv(location, count, reinterpret_cast<const float*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector2i* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform2iv)
			glProgramUniform2iv(m_program, location, count, reinterpret_cast<const int*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform2iv(location, count, reinterpret_cast<const int*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector3d* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform3dv)
			glProgramUniform3dv(m_program, location, count, reinterpret_cast<const double*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform3dv(location, count, reinterpret_cast<const double*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector3f* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform3fv)
			glProgramUniform3fv(m_program, location, count, reinterpret_cast<const float*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform3fv(location, count, reinterpret_cast<const float*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector3i* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform3iv)
			glProgramUniform3iv(m_program, location, count, reinterpret_cast<const int*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform3iv(location, count, reinterpret_cast<const int*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector4d* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform4dv)
			glProgramUniform4dv(m_program, location, count, reinterpret_cast<const double*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4dv(location, count, reinterpret_cast<const double*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector4f* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform4fv)
			glProgramUniform4fv(m_program, location, count, reinterpret_cast<const float*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4fv(location, count, reinterpret_cast<const float*>(vectors));
		}
	}

	void Shader::SendVectorArray(int location, const Vector4i* vectors, unsigned int count) const
	{
		if (location == -1)
			return;

		if (glProgramUniform4iv)
			glProgramUniform4iv(m_program, location, count, reinterpret_cast<const int*>(vectors));
		else
		{
			OpenGL::BindProgram(m_program);
			glUniform4iv(location, count, reinterpret_cast<const int*>(vectors));
		}
	}

	bool Shader::Validate() const
	{
		#if NAZARA_RENDERER_SAFE
		if (!m_program)
		{
			NazaraError("Shader is not initialized");
			return false;
		}
		#endif

		glValidateProgram(m_program);

		GLint success;
		glGetProgramiv(m_program, GL_VALIDATE_STATUS, &success);

		if (success == GL_TRUE)
			return true;
		else
		{
			NazaraError("Failed to validate shader: " + GetLog());
			return false;
		}
	}


	unsigned int Shader::GetOpenGLID() const
	{
		return m_program;
	}

	bool Shader::IsStageSupported(ShaderStageType stage)
	{
		return ShaderStage::IsSupported(stage);
	}

	bool Shader::PostLinkage()
	{
		GLint success;
		glGetProgramiv(m_program, GL_LINK_STATUS, &success);

		m_linked = (success == GL_TRUE);
		if (m_linked)
		{
			// Pour éviter de se tromper entre le nom et la constante
			#define CacheUniform(name) m_uniformLocations[ShaderUniform_##name] = glGetUniformLocation(m_program, #name)

			CacheUniform(InvProjMatrix);
			CacheUniform(InvTargetSize);
			CacheUniform(InvViewMatrix);
			CacheUniform(InvViewProjMatrix);
			CacheUniform(InvWorldMatrix);
			CacheUniform(InvWorldViewMatrix);
			CacheUniform(InvWorldViewProjMatrix);
			CacheUniform(ProjMatrix);
			CacheUniform(TargetSize);
			CacheUniform(ViewMatrix);
			CacheUniform(ViewProjMatrix);
			CacheUniform(WorldMatrix);
			CacheUniform(WorldViewMatrix);
			CacheUniform(WorldViewProjMatrix);

			#undef CacheUniform

			OnShaderUniformInvalidated(this);

			return true;
		}
		else
		{
			NazaraError("Failed to link shader: " + GetLog());
			return false;
		}
	}

	bool Shader::Initialize()
	{
		if (!ShaderLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		return true;
	}

	void Shader::Uninitialize()
	{
		ShaderLibrary::Uninitialize();
	}

	ShaderLibrary::LibraryMap Shader::s_library;
}
