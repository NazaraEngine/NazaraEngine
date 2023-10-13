// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ELEMENTRENDERER_HPP
#define NAZARA_GRAPHICS_ELEMENTRENDERER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Graphics/RenderElementPool.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <NazaraUtils/SparsePtr.hpp>
#include <array>
#include <memory>
#include <optional>
#include <vector>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderElement;
	class RenderFrame;
	class Texture;
	class ViewerInstance;
	struct ElementRendererData;

	class NAZARA_GRAPHICS_API ElementRenderer
	{
		public:
			struct RenderStates;

			ElementRenderer() = default;
			virtual ~ElementRenderer();

			virtual RenderElementPoolBase& GetPool() = 0;

			virtual std::unique_ptr<ElementRendererData> InstanciateData() = 0;
			virtual void Prepare(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, RenderFrame& currentFrame, std::size_t elementCount, const Pointer<const RenderElement>* elements, SparsePtr<const RenderStates> renderStates);
			virtual void PrepareEnd(RenderFrame& currentFrame, ElementRendererData& rendererData);
			virtual void Render(const ViewerInstance& viewerInstance, ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, std::size_t elementCount, const Pointer<const RenderElement>* elements) = 0;
			virtual void Reset(ElementRendererData& rendererData, RenderFrame& currentFrame);

			struct RenderStates
			{
				RenderStates()
				{
					shadowMapsDirectional.fill(nullptr);
					shadowMapsPoint.fill(nullptr);
					shadowMapsSpot.fill(nullptr);
				}

				std::array<const Texture*, PredefinedLightData::MaxLightCount> shadowMapsDirectional;
				std::array<const Texture*, PredefinedLightData::MaxLightCount> shadowMapsPoint;
				std::array<const Texture*, PredefinedLightData::MaxLightCount> shadowMapsSpot;
				RenderBufferView lightData;
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
