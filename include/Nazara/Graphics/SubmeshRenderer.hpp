// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
#define NAZARA_GRAPHICS_SUBMESHRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
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
			SubmeshRenderer() = default;
			~SubmeshRenderer() = default;

			RenderElementPool<RenderSubmesh>& GetPool() override;

			std::unique_ptr<ElementRendererData> InstanciateData() override;
			void Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderFrame& currentFrame, std::size_t elementCount, const Pointer<const RenderElement>* elements, const RenderStates* renderStates) override;
			void Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) override;
			void Reset(ElementRendererData& rendererData, RenderFrame& currentFrame) override;

		private:
			std::vector<ShaderBinding::Binding> m_bindingCache;
			std::vector<ShaderBinding::TextureBinding> m_textureBindingCache;
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

		struct DrawCallIndices
		{
			std::size_t start;
			std::size_t count;
		};

		std::unordered_map<const RenderSubmesh*, DrawCallIndices> drawCallPerElement;
		std::vector<DrawCall> drawCalls;
		std::vector<ShaderBindingPtr> shaderBindings;
	};
}

#include <Nazara/Graphics/SubmeshRenderer.inl>

#endif // NAZARA_GRAPHICS_SUBMESHRENDERER_HPP
