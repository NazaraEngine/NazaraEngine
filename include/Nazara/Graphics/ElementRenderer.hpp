// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ELEMENTRENDERER_HPP
#define NAZARA_ELEMENTRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class CommandBufferBuilder;
	class RenderElement;
	class RenderFrame;
	struct ElementRendererData;

	class NAZARA_GRAPHICS_API ElementRenderer
	{
		public:
			ElementRenderer() = default;
			virtual ~ElementRenderer();

			virtual std::unique_ptr<ElementRendererData> InstanciateData() = 0;
			virtual void Prepare(ElementRendererData& rendererData, RenderFrame& currentFrame, const Pointer<const RenderElement>* elements, std::size_t elementCount);
			virtual void Render(ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) = 0;
			virtual void Reset(ElementRendererData& rendererData, RenderFrame& currentFrame);
	};

	struct NAZARA_GRAPHICS_API ElementRendererData
	{
		ElementRendererData() = default;
		virtual ~ElementRendererData();
	};
}

#include <Nazara/Graphics/ElementRenderer.inl>

#endif
