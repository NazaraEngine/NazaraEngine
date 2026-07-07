// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	template<typename T, typename... Args>
	RenderElementOwner ElementRendererRegistry::AllocateElement(Args&&... args)
	{
		ElementRenderer& elementRenderer = GetElementRenderer(SafeCast<std::size_t>(T::ElementType));
		RenderElementPool<T>& pool = SafeCast<RenderElementPool<T>&>(elementRenderer.GetPool());

		return pool.Allocate(std::forward<Args>(args)...);
	}

	inline ElementRenderer& ElementRendererRegistry::GetElementRenderer(std::size_t elementIndex)
	{
		assert(elementIndex < m_elementRenderers.size());
		return *m_elementRenderers[elementIndex];
	}

	inline std::size_t ElementRendererRegistry::GetElementRendererCount() const
	{
		return m_elementRenderers.size();
	}

	template<typename F>
	void ElementRendererRegistry::ForEachElementRenderer(F&& callback)
	{
		for (std::size_t i = 0; i < m_elementRenderers.size(); ++i)
		{
			if (m_elementRenderers[i])
				callback(i, *m_elementRenderers[i]);
		}
	}

	template<typename T>
	void ElementRendererRegistry::RegisterElementRenderer(std::unique_ptr<ElementRenderer> renderer)
	{
		return RegisterElementRenderer(SafeCast<std::size_t>(T::ElementType), std::move(renderer));
	}

	inline void ElementRendererRegistry::RegisterElementRenderer(std::size_t elementIndex, std::unique_ptr<ElementRenderer> renderer)
	{
		if (elementIndex >= m_elementRenderers.size())
			m_elementRenderers.resize(elementIndex + 1);

		assert(!m_elementRenderers[elementIndex]);
		m_elementRenderers[elementIndex] = std::move(renderer);
	}
}
