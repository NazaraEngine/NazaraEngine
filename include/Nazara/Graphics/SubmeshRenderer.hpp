// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
#define NAZARA_GRAPHICS_SUBMESHRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/RenderResourceReferences.hpp>
#include <Nazara/Graphics/RenderSubmesh.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/DrawIndirect.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>

namespace Nz
{
	class RenderPipeline;
	class ShaderBinding;

	class NAZARA_GRAPHICS_API SubmeshRenderer final : public ElementRenderer
	{
		public:
			SubmeshRenderer(RenderDevice& device);
			~SubmeshRenderer() = default;

			void ForEachIndirectBuffer(ElementRendererData& rendererData, FunctionRef<void(GpuBuffer& buffer, std::size_t commandCount)> callback) override;

			RenderElementPool<RenderSubmesh>& GetPool() override;

			std::unique_ptr<ElementRendererData> InstanciateData() override;

			void Prepare(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void Render(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void Reset(ElementRendererData& rendererData, RenderResources& renderResources) override;

		private:
			struct PoolData
			{
				std::vector<RenderResourceReferences> references;
				std::vector<std::shared_ptr<GpuBuffer>> indirectBuffers;
			};

			static constexpr UInt64 IndirectCommandBufferCount = 10 * 1024;

			std::shared_ptr<PoolData> m_pool;
			std::vector<ShaderBinding::Binding> m_bindingCache;
			RenderElementPool<RenderSubmesh> m_submeshPool;
			RenderDevice& m_device;
	};

	struct SubmeshRendererData : public ElementRendererData
	{
		std::optional<RenderResourceReferences> references;
		std::size_t drawIndirectBufferIndex = 0;
		std::size_t totalElementCount = 0;
		std::vector<ShaderBindingPtr> shaderBindings;
		std::vector<std::shared_ptr<GpuBuffer>> drawIndirectBuffers;
		UInt8* currentIndirectBufferPtr = nullptr;
		UInt32 drawElementCounter = 0;
		UInt32 indirectCommandIndex = 0;
	};
}

#include <Nazara/Graphics/SubmeshRenderer.inl>

#endif // NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
