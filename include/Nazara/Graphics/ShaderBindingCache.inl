// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void ShaderBindingCache::DestroyViewerCache(const ViewerInstance& viewerInstance)
	{
		m_viewerBindings.erase(&viewerInstance);
	}

	template<typename F>
	ShaderBinding* ShaderBindingCache::GetSceneBinding(std::size_t setHash, F&& createFunctor)
	{
		auto it = m_sceneBindings.find(setHash);
		if (it != m_sceneBindings.end())
			return it->second.get();

		// Entry does not exist, generate it
		return m_sceneBindings.emplace(setHash, createFunctor()).first->second.get();
	}

	template<typename F>
	ShaderBinding* ShaderBindingCache::GetViewerBinding(const ViewerInstance& viewerInstance, std::size_t setHash, F&& createFunctor)
	{
		ViewerEntry& viewerEntry = m_viewerBindings[&viewerInstance];
		auto it = viewerEntry.bindings.find(setHash);
		if (it != viewerEntry.bindings.end())
			return it->second.get();

		// Entry does not exist, generate it
		return viewerEntry.bindings.emplace(setHash, createFunctor()).first->second.get();
	}

	inline void ShaderBindingCache::InvalidateSceneBindings()
	{
		m_sceneBindings.clear();
	}

	inline void ShaderBindingCache::InvalidateViewerBindings(const ViewerInstance& viewerInstance)
	{
		auto viewerIt = m_viewerBindings.find(&viewerInstance);
		if (viewerIt == m_viewerBindings.end())
			return;

		ViewerEntry& viewerEntry = viewerIt->second;
		viewerEntry.bindings.clear();
	}
}

