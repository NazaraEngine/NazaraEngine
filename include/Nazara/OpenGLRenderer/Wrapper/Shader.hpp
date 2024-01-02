// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_SHADER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_SHADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>
#include <NazaraUtils/MovableValue.hpp>

namespace Nz::GL
{
	class Shader : public DeviceObject<Shader, GL_SHADER, GLenum>
	{
		friend DeviceObject;

		public:
			using DeviceObject::DeviceObject;
			Shader(const Shader&) = delete;
			Shader(Shader&&) noexcept = default;
			~Shader() = default;

			inline void Compile();

			inline bool GetCompilationStatus(std::string* error = nullptr) const;
			inline std::string GetSource() const;

			inline void SetBinarySource(GLenum binaryFormat, const void* binary, GLsizei length);
			inline void SetSource(const char* source, GLint length);
			inline void SetSource(std::string_view source);

			// GL_ARB_gl_spirv
			inline void SpecializeShader(const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue);

			Shader& operator=(const Shader&) = delete;
			Shader& operator=(Shader&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context, GLenum shaderStage);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Shader.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_SHADER_HPP
