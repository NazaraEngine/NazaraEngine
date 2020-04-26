// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const RenderPipelineLayoutInfo& OpenGLRenderPipelineLayout::GetLayoutInfo() const
	{
		return m_layoutInfo;
	}

	inline std::size_t OpenGLRenderPipelineLayout::GetTextureDescriptorCount() const
	{
		return m_textureDescriptorCount;
	}

	inline std::size_t OpenGLRenderPipelineLayout::GetUniformBufferDescriptorCount() const
	{
		return m_uniformBufferDescriptorCount;
	}

	inline void OpenGLRenderPipelineLayout::TryToShrink()
	{
		std::size_t poolCount = m_descriptorPools.size();
		if (poolCount >= 2 && m_descriptorPools.back().freeBindings.TestAll())
		{
			for (std::size_t i = poolCount - 1; i > 0; ++i)
			{
				if (m_descriptorPools[i].freeBindings.TestAll())
					poolCount--;
			}

			m_descriptorPools.resize(poolCount);
		}
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
