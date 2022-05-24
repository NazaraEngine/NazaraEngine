// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_SAMPLER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_SAMPLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>
#include <Nazara/Utils/MovableValue.hpp>

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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_SAMPLER_HPP
