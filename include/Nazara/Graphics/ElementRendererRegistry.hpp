// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ELEMENTRENDERERREGISTRY_HPP
#define NAZARA_GRAPHICS_ELEMENTRENDERERREGISTRY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/RenderElement.hpp>
#include <Nazara/Graphics/RenderElementPool.hpp>
#include <Nazara/Graphics/RenderQueue.hpp>
#include <memory>

namespace Nz
{
	class ElementRenderer;

	class NAZARA_GRAPHICS_API ElementRendererRegistry
	{
		public:
			ElementRendererRegistry();
			ElementRendererRegistry(const ElementRendererRegistry&) = delete;
			ElementRendererRegistry(ElementRendererRegistry&&) = delete;
			~ElementRendererRegistry() = default;

			template<typename T, typename... Args> RenderElementOwner AllocateElement(Args&&... args);

			template<typename F> void ForEachElementRenderer(F&& callback);

			inline ElementRenderer& GetElementRenderer(std::size_t elementIndex);
			inline std::size_t GetElementRendererCount() const;

			template<typename F> void ProcessRenderQueue(const RenderQueue<const RenderElement*>& renderQueue, F&& callback);

			template<typename T> void RegisterElementRenderer(std::unique_ptr<ElementRenderer> renderer);
			inline void RegisterElementRenderer(std::size_t elementIndex, std::unique_ptr<ElementRenderer> renderer);

			ElementRendererRegistry& operator=(const ElementRendererRegistry&) = delete;
			ElementRendererRegistry& operator=(ElementRendererRegistry&&) = delete;

		private:
			std::vector<std::unique_ptr<ElementRenderer>> m_elementRenderers;
	};
}

#include <Nazara/Graphics/ElementRendererRegistry.inl>

#endif // NAZARA_GRAPHICS_ELEMENTRENDERERREGISTRY_HPP
