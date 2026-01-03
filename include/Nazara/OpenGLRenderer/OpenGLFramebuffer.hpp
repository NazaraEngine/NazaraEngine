// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLFRAMEBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLFRAMEBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/Renderer/Framebuffer.hpp>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLFramebuffer : public Framebuffer
	{
		public:
			using Framebuffer::Framebuffer;

			virtual void Activate() const = 0;

			virtual std::size_t GetColorBufferCount() const = 0;

			virtual const Vector2ui& GetSize() const = 0;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLFramebuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLFRAMEBUFFER_HPP
