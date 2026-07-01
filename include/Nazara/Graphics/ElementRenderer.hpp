// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ELEMENTRENDERER_HPP
#define NAZARA_GRAPHICS_ELEMENTRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderElementPool.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <array>
#include <memory>
#include <optional>
#include <vector>

namespace Nz
{
	class AbstractViewer;
	class CommandBufferBuilder;
	class Material;
	class RenderElement;
	class RenderResources;
	class Texture;
	class TextureSampler;
	struct ElementRendererData;

	class NAZARA_GRAPHICS_API ElementRenderer
	{
		public:
			struct RenderData;

			ElementRenderer() = default;
			virtual ~ElementRenderer();

			virtual RenderElementPoolBase& GetPool() = 0;

			virtual std::unique_ptr<ElementRendererData> InstanciateData() = 0;
			virtual void Prepare(const RenderData& renderData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, std::size_t elementCount, const Pointer<const RenderElement>* elements);
			virtual void PrepareEnd(ElementRendererData& rendererData, RenderResources& renderResources, CommandBufferBuilder& commandBuffer);
			virtual void Render(const RenderData& renderData, const AbstractViewer& viewer, ElementRendererData& rendererData, RenderResources& renderResources, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) = 0;
			virtual void Reset(ElementRendererData& rendererData, RenderResources& renderResources);

			struct RenderData
			{
				const Texture* shadowAtlas = nullptr;
				Recti renderRegion;
				RenderBufferView directionalLights;
				RenderBufferView directionalLightAtlasMapping;
				RenderBufferView pointLights;
				RenderBufferView pointLightAtlasMapping;
				RenderBufferView spotLights;
				RenderBufferView spotLightAtlasMapping;
			};

		protected:
			void FillSceneBindings(const RenderData& renderData, const Material& material, std::vector<ShaderBinding::Binding>& bindings);
	};

	struct NAZARA_GRAPHICS_API ElementRendererData
	{
		ElementRendererData() = default;
		virtual ~ElementRendererData();
	};
}

#include <Nazara/Graphics/ElementRenderer.inl>

#endif // NAZARA_GRAPHICS_ELEMENTRENDERER_HPP
