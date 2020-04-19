// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLSHADER_HPP
#define NAZARA_OPENGLRENDERER_GLSHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>

namespace Nz::GL
{
	class Shader
	{
		public:
			Shader() = default;
			Shader(const Shader&) = delete;
			Shader(Shader&&) noexcept = default;
			inline ~Shader();

			inline void Compile();

			inline bool Create(OpenGLDevice& device, GLenum type);
			inline void Destroy();

			inline bool GetCompilationStatus(std::string* error = nullptr);

			inline void SetBinarySource(GLenum binaryFormat, const void* binary, GLsizei length);
			inline void SetSource(const char* source, GLint length);

			// GL_ARB_gl_spirv
			inline void SpecializeShader(const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue);

			Shader& operator=(const Shader&) = delete;
			Shader& operator=(Shader&&) noexcept = default;

		private:
			MovablePtr<OpenGLDevice> m_device;
			MovableValue<GLuint> m_shader;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Shader.inl>

#endif
