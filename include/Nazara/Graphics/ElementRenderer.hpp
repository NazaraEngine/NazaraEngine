// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ELEMENTRENDERER_HPP
#define NAZARA_GRAPHICS_ELEMENTRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderElementPool.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class AbstractViewer;
	class GpuCommandBufferBuilder;
	class Material;
	class GpuBuffer;
	class RenderElement;
	class GpuResources;
	class ShaderBindingCache;
	class Texture;
	class TextureSampler;
	struct ElementRendererData;

	class NAZARA_GRAPHICS_API ElementRenderer
	{
		public:
			struct RenderData;
			struct SceneData;

			ElementRenderer() = default;
			virtual ~ElementRenderer();

			virtual void ForEachIndirectBuffer(ElementRendererData& rendererData, FunctionRef<void(GpuBuffer& buffer, std::size_t commandCount)> callback) = 0;

			virtual RenderElementPoolBase& GetPool() = 0;

			virtual std::unique_ptr<ElementRendererData> InstanciateData() = 0;

			virtual void Prepare(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, GpuResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements);
			virtual void PrepareEnd(ElementRendererData& rendererData, GpuResources& renderResources, GpuCommandBufferBuilder& commandBuffer);
			virtual void Render(const RenderData& renderData, const SceneData& sceneData, const AbstractViewer& viewer, ElementRendererData& rendererData, GpuResources& renderResources, GpuCommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) = 0;
			virtual void Reset(ElementRendererData& rendererData, GpuResources& renderResources);

			struct RenderData
			{
				Recti renderRegion;
				ShaderBindingCache* shaderBindingCache;
			};

			struct SceneData
			{
				std::shared_ptr<GpuBuffer> directionalLights = {};
				std::shared_ptr<GpuBuffer> directionalLightAtlasMapping = {};
				std::shared_ptr<GpuBuffer> instanceBuffer;
				std::shared_ptr<GpuBuffer> pointLights = {};
				std::shared_ptr<GpuBuffer> pointLightAtlasMapping = {};
				std::shared_ptr<GpuBuffer> spotLights = {};
				std::shared_ptr<GpuBuffer> spotLightAtlasMapping = {};
				std::shared_ptr<Texture> shadowAtlas = {};
			};
	};

	struct NAZARA_GRAPHICS_API ElementRendererData
	{
		ElementRendererData() = default;
		virtual ~ElementRendererData();
	};
}

#include <Nazara/Graphics/ElementRenderer.inl>

#endif // NAZARA_GRAPHICS_ELEMENTRENDERER_HPP
