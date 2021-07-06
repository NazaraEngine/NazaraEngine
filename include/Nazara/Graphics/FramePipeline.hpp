// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FRAMEPIPELINE_HPP
#define NAZARA_FRAMEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class AbstractViewer;
	class InstancedRenderable;
	class RenderFrame;
	class WorldInstance;

	class NAZARA_GRAPHICS_API FramePipeline
	{
		public:
			FramePipeline() = default;
			FramePipeline(const FramePipeline&) = delete;
			FramePipeline(FramePipeline&&) noexcept = default;
			virtual ~FramePipeline();

			virtual void InvalidateViewer(AbstractViewer* viewerInstance) = 0;
			virtual void InvalidateWorldInstance(WorldInstance* worldInstance) = 0;

			virtual void RegisterInstancedDrawable(WorldInstance* worldInstance, const InstancedRenderable* instancedRenderable) = 0;
			virtual void RegisterViewer(AbstractViewer* viewerInstance) = 0;

			virtual void Render(RenderFrame& renderFrame) = 0;

			virtual void UnregisterInstancedDrawable(WorldInstance* worldInstance, const InstancedRenderable* instancedRenderable) = 0;
			virtual void UnregisterViewer(AbstractViewer* viewerInstance) = 0;

			FramePipeline& operator=(const FramePipeline&) = delete;
			FramePipeline& operator=(FramePipeline&&) noexcept = default;
	};
}

#include <Nazara/Graphics/FramePipeline.inl>

#endif
