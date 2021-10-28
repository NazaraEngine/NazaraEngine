// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_DUMMYSURFACE_HPP
#define NAZARA_OPENGLRENDERER_DUMMYSURFACE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API DummySurface : public RenderSurface
	{
		public:
			DummySurface() = default;
			~DummySurface() = default;

			bool Create(WindowHandle handle) override;
			void Destroy() override;

			inline WindowHandle GetWindowHandle() const;

		private:
			WindowHandle m_handle;
	};
}

#include <Nazara/OpenGLRenderer/DummySurface.inl>

#endif // NAZARA_OPENGLRENDERER_DUMMYSURFACE_HPP
