// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLPROGRAM_HPP
#define NAZARA_OPENGLRENDERER_GLPROGRAM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::GL
{
	class Program : public DeviceObject<Program, GL_PROGRAM>
	{
		friend DeviceObject;

		public:
			Program() = default;
			Program(const Program&) = delete;
			Program(Program&&) noexcept = default;
			~Program() = default;

			inline void AttachShader(GLuint shader);

			inline void Get(GLenum pname, GLint* params) const;
			inline void GetActiveUniform(GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) const;
			inline void GetActiveUniformBlock(GLuint uniformBlockIndex, GLenum pname, GLint* params) const;
			inline std::vector<GLint> GetActiveUniformBlockUniformIndices(GLuint uniformBlockIndex) const;
			inline void GetActiveUniformBlockName(GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) const;
			inline std::string GetActiveUniformBlockName(GLuint uniformBlockIndex) const;
			inline std::string GetActiveUniformName(GLuint index) const;
			inline std::vector<GLint> GetActiveUniforms(GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname) const;
			inline void GetActiveUniforms(GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) const;
			inline bool GetLinkStatus(std::string* error = nullptr) const;
			inline GLuint GetUniformBlockIndex(const char* uniformBlockName) const;
			inline GLuint GetUniformBlockIndex(const std::string& uniformBlockName) const;
			inline GLint GetUniformLocation(const char* uniformName) const;
			inline GLint GetUniformLocation(const std::string& uniformName) const;

			inline void Link();

			inline void Uniform(GLint uniformLocation, float value) const;

			Program& operator=(const Program&) = delete;
			Program& operator=(Program&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Program.inl>

#endif
