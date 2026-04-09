// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLASYNCCOMMANDS_HPP
#define NAZARA_OPENGLRENDERER_OPENGLASYNCCOMMANDS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.hpp>
#include <Nazara/Renderer/AsyncRenderCommands.hpp>

namespace Nz
{
	class OpenGLDevice;

	class NAZARA_OPENGLRENDERER_API OpenGLAsyncCommands : public AsyncRenderCommands
	{
		friend OpenGLDevice;

		public:
			OpenGLAsyncCommands() = default;
			OpenGLAsyncCommands(const OpenGLAsyncCommands&) = delete;
			OpenGLAsyncCommands(OpenGLAsyncCommands&&) = delete;
			~OpenGLAsyncCommands() = default;

			void AddCommands(Nz::FunctionRef<void(CommandBufferBuilder& builder)> callback) override;

			void Execute();

			OpenGLAsyncCommands& operator=(const OpenGLAsyncCommands&) = delete;
			OpenGLAsyncCommands& operator=(OpenGLAsyncCommands&&) = delete;

		private:
			using AsyncRenderCommands::TriggerCallbacks;

			OpenGLCommandBuffer m_commandBuffer;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLAsyncCommands.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLASYNCCOMMANDS_HPP
