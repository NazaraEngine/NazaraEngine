// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLShaderBinding::OpenGLShaderBinding(OpenGLRenderPipelineLayout& owner, std::size_t poolIndex, std::size_t bindingIndex, Vk::DescriptorSet descriptorSet) :
	m_descriptorSet(std::move(descriptorSet)),
	m_owner(owner),
	m_bindingIndex(bindingIndex),
	m_poolIndex(poolIndex)
	{
	}

	inline std::size_t OpenGLShaderBinding::GetBindingIndex() const
	{
		return m_bindingIndex;
	}

	inline std::size_t OpenGLShaderBinding::GetPoolIndex() const
	{
		return m_poolIndex;
	}

	inline const Vk::DescriptorSet& OpenGLShaderBinding::GetDescriptorSet() const
	{
		return m_descriptorSet;
	}

	inline const OpenGLRenderPipelineLayout& OpenGLShaderBinding::GetOwner() const
	{
		return m_owner;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
