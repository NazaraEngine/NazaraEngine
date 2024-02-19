// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	template<typename T>
	template<typename... Args>
	RenderElementOwner RenderElementPool<T>::Allocate(Args&&... args)
	{
		std::size_t poolIndex;
		 T* element = m_pool.Allocate(poolIndex, std::forward<Args>(args)...);

		return RenderElementOwner(this, poolIndex, element);
	}

	template<typename T>
	RenderElementPool<T>::RenderElementPool() :
	m_pool(4096) //< TODO: Allow to adjust pool count
	{
	}

	template<typename T>
	void RenderElementPool<T>::Clear()
	{
		m_pool.Clear();
	}

	template<typename T>
	void RenderElementPool<T>::Free(std::size_t index)
	{
		m_pool.Free(index);
	}
}

