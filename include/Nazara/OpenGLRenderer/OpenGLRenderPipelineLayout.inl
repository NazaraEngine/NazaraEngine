// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GlslWriter::BindingMapping& OpenGLRenderPipelineLayout::GetBindingMapping() const
	{
		return m_bindingMapping;
	}

	inline const RenderPipelineLayoutInfo& OpenGLRenderPipelineLayout::GetLayoutInfo() const
	{
		return m_layoutInfo;
	}

	template<typename F>
	void OpenGLRenderPipelineLayout::ForEachDescriptor(std::size_t poolIndex, std::size_t bindingIndex, F&& functor)
	{
		assert(poolIndex < m_descriptorPools.size());
		auto& pool = m_descriptorPools[poolIndex];
		assert(!pool.freeBindings.Test(bindingIndex));

		for (std::size_t descriptorIndex = 0; descriptorIndex < m_maxDescriptorCount; ++descriptorIndex)
		{
			std::visit([&](auto&& arg)
			{
				if constexpr (!std::is_same_v<std::decay_t<decltype(arg)>, std::monostate>)
					functor(UInt32(descriptorIndex), arg);
			},
			pool.descriptors[bindingIndex * m_maxDescriptorCount + descriptorIndex]);
		}
	}

	inline void OpenGLRenderPipelineLayout::TryToShrink()
	{
		std::size_t poolCount = m_descriptorPools.size();
		if (poolCount >= 2 && m_descriptorPools.back().freeBindings.TestAll())
		{
			for (std::size_t i = poolCount - 1; i > 0; --i)
			{
				if (!m_descriptorPools[i].freeBindings.TestAll())
					break;

				poolCount--;
			}

			m_descriptorPools.resize(poolCount);
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
