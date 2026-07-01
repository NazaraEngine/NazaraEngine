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
#include <Nazara/Renderer/ShaderBinding.hpp>

namespace Nz
{
	class RenderPipeline;
	class ShaderBinding;

	class NAZARA_GRAPHICS_API SubmeshRenderer final : public ElementRenderer
	{
		public:
			SubmeshRenderer();
			~SubmeshRenderer() = default;

			RenderElementPool<RenderSubmesh>& GetPool() override;

			std::unique_ptr<ElementRendererData> InstanciateData() override;
			void Render(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void Reset(ElementRendererData& rendererData, RenderResources& renderResources) override;

		private:
			struct PoolData
			{
				std::vector<RenderResourceReferences> references;
			};

			std::shared_ptr<PoolData> m_pool;
			std::vector<ShaderBinding::Binding> m_bindingCache;
			RenderElementPool<RenderSubmesh> m_submeshPool;
	};

	struct SubmeshRendererData : public ElementRendererData
	{
		struct DrawCall
		{
			const RenderBuffer* indexBuffer;
			const RenderBuffer* vertexBuffer;
			const RenderPipeline* renderPipeline;
			const ShaderBinding* shaderBinding;
			std::size_t firstIndex;
			std::size_t indexCount;
			IndexType indexType;
			Recti scissorBox;
		};

		std::optional<RenderResourceReferences> references;
		std::vector<ShaderBindingPtr> shaderBindings;
	};
}

#include <Nazara/Graphics/SubmeshRenderer.inl>

#endif // NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
