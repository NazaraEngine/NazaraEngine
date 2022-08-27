// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Rect.hpp>

namespace Nz
{
	class InstancedRenderable;
	class SkeletonInstance;
	class WorldInstance;

	class NAZARA_GRAPHICS_API FramePipelinePass
	{
		public:
			FramePipelinePass() = default;
			FramePipelinePass(const FramePipelinePass&) = delete;
			FramePipelinePass(FramePipelinePass&&) = delete;
			virtual ~FramePipelinePass();

			FramePipelinePass& operator=(const FramePipelinePass&) = delete;
			FramePipelinePass& operator=(FramePipelinePass&&) = delete;

			struct VisibleRenderable
			{
				const InstancedRenderable* instancedRenderable;
				const SkeletonInstance* skeletonInstance;
				const WorldInstance* worldInstance;
				Recti scissorBox;
			};
	};
}

#include <Nazara/Graphics/FramePipelinePass.inl>

#endif // NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP
