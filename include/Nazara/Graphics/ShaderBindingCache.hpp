// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_SHADERBINDINGCACHE_HPP
#define NAZARA_GRAPHICS_SHADERBINDINGCACHE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/ViewerInstance.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <unordered_map>

namespace Nz
{
	class ShaderBindingCache
	{
		public:
			ShaderBindingCache() = default;
			ShaderBindingCache(const ShaderBindingCache&) = delete;
			ShaderBindingCache(ShaderBindingCache&&) = delete;
			~ShaderBindingCache() = default;

			inline void DestroyViewerCache(const ViewerInstance& viewerInstance);

			template<typename F> ShaderBinding* GetSceneBinding(std::size_t setHash, F&& createFunctor);
			template<typename F> ShaderBinding* GetViewerBinding(const ViewerInstance& viewerInstance, std::size_t setHash, F&& createFunctor);

			inline void InvalidateSceneBindings();
			inline void InvalidateViewerBindings(const ViewerInstance& viewerInstance);

			ShaderBindingCache& operator=(const ShaderBindingCache&) = delete;
			ShaderBindingCache& operator=(ShaderBindingCache&&) = delete;

		private:
			struct ViewerEntry
			{
				std::unordered_map<std::size_t, ShaderBindingPtr> bindings;
			};

			std::unordered_map<std::size_t, ShaderBindingPtr> m_sceneBindings;
			std::unordered_map<const ViewerInstance*, ViewerEntry> m_viewerBindings;
	};
}

#include <Nazara/Graphics/ShaderBindingCache.inl>

#endif // NAZARA_GRAPHICS_SHADERBINDINGCACHE_HPP
