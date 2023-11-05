// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPIPELINE_HPP
#define NAZARA_GRAPHICS_FRAMEPIPELINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/SkeletonInstance.hpp>
#include <Nazara/Graphics/WorldInstance.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class InstancedRenderable;
	class Light;
	class LightShadowData;
	class MaterialInstance;
	class PipelineViewer;
	class RenderFrame;

	class NAZARA_GRAPHICS_API FramePipeline
	{
		public:
			FramePipeline();
			FramePipeline(const FramePipeline&) = delete;
			FramePipeline(FramePipeline&&) noexcept = default;
			virtual ~FramePipeline();

			// TODO: Move RenderQueue handling to proper classes (allowing to reuse them)
			virtual const std::vector<FramePipelinePass::VisibleRenderable>& FrustumCull(const Frustumf& frustum, UInt32 mask, std::size_t& visibilityHash) const = 0;

			virtual void ForEachRegisteredMaterialInstance(FunctionRef<void(const MaterialInstance& materialInstance)> callback) = 0;

			inline DebugDrawer& GetDebugDrawer();

			virtual void QueueTransfer(TransferInterface* transfer) = 0;

			virtual std::size_t RegisterLight(const Light* light, UInt32 renderMask) = 0;
			virtual std::size_t RegisterRenderable(std::size_t worldInstanceIndex, std::size_t skeletonInstanceIndex, const InstancedRenderable* instancedRenderable, UInt32 renderMask, const Recti& scissorBox) = 0;
			virtual std::size_t RegisterSkeleton(SkeletonInstancePtr skeletonInstance) = 0;
			virtual std::size_t RegisterViewer(PipelineViewer* viewerInstance, Int32 renderOrder) = 0;
			virtual std::size_t RegisterWorldInstance(WorldInstancePtr worldInstance) = 0;

			virtual const Light* RetrieveLight(std::size_t lightIndex) const = 0;
			virtual const LightShadowData* RetrieveLightShadowData(std::size_t lightIndex) const = 0;
			virtual const Texture* RetrieveLightShadowmap(std::size_t lightIndex, const AbstractViewer* viewer) const = 0;

			virtual void Render(RenderFrame& renderFrame) = 0;

			virtual void UnregisterLight(std::size_t lightIndex) = 0;
			virtual void UnregisterRenderable(std::size_t renderableIndex) = 0;
			virtual void UnregisterSkeleton(std::size_t skeletonIndex) = 0;
			virtual void UnregisterViewer(std::size_t viewerIndex) = 0;
			virtual void UnregisterWorldInstance(std::size_t worldInstance) = 0;

			virtual void UpdateLightRenderMask(std::size_t lightIndex, UInt32 renderMask) = 0;
			virtual void UpdateRenderableRenderMask(std::size_t renderableIndex, UInt32 renderMask) = 0;
			virtual void UpdateRenderableScissorBox(std::size_t renderableIndex, const Recti& scissorBox) = 0;
			virtual void UpdateRenderableSkeletonInstance(std::size_t renderableIndex, std::size_t skeletonIndex) = 0;
			virtual void UpdateViewerRenderOrder(std::size_t viewerIndex, Int32 renderOrder) = 0;

			FramePipeline& operator=(const FramePipeline&) = delete;
			FramePipeline& operator=(FramePipeline&&) = delete;

			NazaraSignal(OnTransfer, FramePipeline* /*pipeline*/, RenderFrame& /*renderFrame*/, CommandBufferBuilder& /*builder*/);

			static constexpr std::size_t NoSkeletonInstance = std::numeric_limits<std::size_t>::max();

		private:
			DebugDrawer m_debugDrawer;
	};
}

#include <Nazara/Graphics/FramePipeline.inl>

#endif // NAZARA_GRAPHICS_FRAMEPIPELINE_HPP
