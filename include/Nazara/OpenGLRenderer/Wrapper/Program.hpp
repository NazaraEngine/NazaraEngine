// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLSHADER_HPP
#define NAZARA_OPENGLRENDERER_GLSHADER_HPP

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

			inline bool GetLinkStatus(std::string* error = nullptr);

			inline void Link();

			Program& operator=(const Program&) = delete;
			Program& operator=(Program&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Program.inl>

#endif
