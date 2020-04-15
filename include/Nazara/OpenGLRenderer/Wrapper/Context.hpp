// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_CONTEXT_HPP
#define NAZARA_OPENGLRENDERER_CONTEXT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/GLContext.hpp>
#include <memory>

namespace Nz::GL
{
	class Context
	{
		public:
			Context() = default;
			Context(const Context&) = delete;
			Context(Context&& object) noexcept = default;
			~Context() = default;

			Context& operator=(const Context&) = delete;
			Context& operator=(Context&& object) noexcept = default;

		private:
			std::unique_ptr<GLContext> m_impl;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Context.inl>

#endif
