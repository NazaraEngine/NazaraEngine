// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SUBMESHRENDERER_HPP
#define NAZARA_SUBMESHRENDERER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API SubmeshRenderer : public ElementRenderer
	{
		public:
			SubmeshRenderer();
			~SubmeshRenderer() = default;

			std::unique_ptr<ElementRendererData> InstanciateData();
			void Render(ElementRendererData& rendererData, CommandBufferBuilder& commandBuffer, const Pointer<const RenderElement>* elements, std::size_t elementCount) override;

		private:
			ShaderBindingPtr m_renderDataBinding;
	};
}

#include <Nazara/Graphics/SubmeshRenderer.inl>

#endif
