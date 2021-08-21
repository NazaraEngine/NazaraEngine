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

	class NAZARA_GRAPHICS_API ElementRenderer
	{
		public:
			ElementRenderer() = default;
			virtual ~ElementRenderer();

			virtual void Render(CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) = 0;
	};
}

#include <Nazara/Graphics/ElementRenderer.inl>

#endif
