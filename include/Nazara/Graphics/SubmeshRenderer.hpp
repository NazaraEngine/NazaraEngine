// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SUBMESHRENDERER_HPP
#define NAZARA_SUBMESHRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API SubmeshRenderer : public ElementRenderer
	{
		public:
			SubmeshRenderer() = default;
			~SubmeshRenderer() = default;

			void Render(CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;
	};
}

#include <Nazara/Graphics/SubmeshRenderer.inl>

#endif
