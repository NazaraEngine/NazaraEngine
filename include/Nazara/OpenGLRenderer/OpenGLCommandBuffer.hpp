// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLCommandBuffer final : public CommandBuffer
	{
		public:
			inline OpenGLCommandBuffer();
			OpenGLCommandBuffer(const OpenGLCommandBuffer&) = delete;
			OpenGLCommandBuffer(OpenGLCommandBuffer&&) noexcept = default;
			~OpenGLCommandBuffer() = default;

			OpenGLCommandBuffer& operator=(const OpenGLCommandBuffer&) = delete;
			OpenGLCommandBuffer& operator=(OpenGLCommandBuffer&&) = delete;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
