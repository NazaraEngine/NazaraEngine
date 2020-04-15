// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_SURFACE_HPP
#define NAZARA_OPENGLRENDERER_SURFACE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Surface.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Swapchain.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLSurface : public RenderSurface
	{
		public:
			OpenGLSurface();
			OpenGLSurface(const OpenGLSurface&) = delete;
			OpenGLSurface(OpenGLSurface&&) = delete; ///TODO
			virtual ~OpenGLSurface();

			bool Create(WindowHandle handle) override;
			void Destroy() override;

			inline Vk::Surface& GetSurface();

			OpenGLSurface& operator=(const OpenGLSurface&) = delete;
			OpenGLSurface& operator=(OpenGLSurface&&) = delete; ///TODO

		private:
			Vk::Surface m_surface;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLSurface.inl>

#endif // NAZARA_OPENGLRENDERER_SURFACE_HPP
