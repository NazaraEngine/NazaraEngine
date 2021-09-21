// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLSAMPLER_HPP
#define NAZARA_OPENGLRENDERER_GLSAMPLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::GL
{
	class Sampler : public DeviceObject<Sampler, GL_SAMPLER>
	{
		friend DeviceObject;

		public:
			using DeviceObject::DeviceObject;
			Sampler(const Sampler&) = delete;
			Sampler(Sampler&&) noexcept = default;
			~Sampler() = default;

			inline void SetParameterf(GLenum pname, GLfloat param);
			inline void SetParameteri(GLenum pname, GLint param);
			inline void SetParameterfv(GLenum pname, const GLfloat* param);
			inline void SetParameteriv(GLenum pname, const GLint* param);

			Sampler& operator=(const Sampler&) = delete;
			Sampler& operator=(Sampler&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Sampler.inl>

#endif
