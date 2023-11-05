// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Rect.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <limits>

namespace Nz
{
	class AbstractViewer;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class InstancedRenderable;
	class MaterialInstance;
	class RenderFrame;
	class SkeletonInstance;
	class WorldInstance;

	class NAZARA_GRAPHICS_API FramePipelinePass
	{
		public:
			struct FrameData;
			struct PassData;
			struct PassInputOuputs;
			struct VisibleRenderable;

			inline FramePipelinePass(FramePipelineNotificationFlags notificationFlags);
			FramePipelinePass(const FramePipelinePass&) = delete;
			FramePipelinePass(FramePipelinePass&&) = delete;
			virtual ~FramePipelinePass();

			virtual void InvalidateElements();

			virtual void Prepare(FrameData& frameData) = 0;

			virtual void RegisterMaterialInstance(const MaterialInstance& materialInstance);

			virtual FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) = 0;

			inline bool ShouldNotify(FramePipelineNotification notification) const;

			virtual void UnregisterMaterialInstance(const MaterialInstance& materialInstance);

			FramePipelinePass& operator=(const FramePipelinePass&) = delete;
			FramePipelinePass& operator=(FramePipelinePass&&) = delete;

			struct FrameData
			{
				const Bitset<UInt64>* visibleLights;
				const Frustumf& frustum;
				RenderFrame& renderFrame;
				const std::vector<VisibleRenderable>& visibleRenderables;
				std::size_t visibilityHash;
			};

			struct PassData
			{
				AbstractViewer* viewer;
				ElementRendererRegistry& elementRegistry;
				FramePipeline& pipeline;
			};

			struct PassInputOuputs
			{
				// TODO: Add Nz::View / Nz::Span
				const std::size_t* inputAttachments;
				const std::size_t* outputAttachments;
				std::size_t depthStencilInput = InvalidAttachmentIndex;
				std::size_t depthStencilOutput = InvalidAttachmentIndex;
				std::size_t inputCount = 0;
				std::size_t outputCount = 0;
			};

			struct VisibleRenderable
			{
				const InstancedRenderable* instancedRenderable;
				const SkeletonInstance* skeletonInstance;
				const WorldInstance* worldInstance;
				Recti scissorBox;
			};

			static constexpr std::size_t InvalidAttachmentIndex = std::numeric_limits<std::size_t>::max();

		private:
			FramePipelineNotificationFlags m_notificationFlags;
	};
}

#include <Nazara/Graphics/FramePipelinePass.inl>

#endif // NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP
