// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline OpenGLShaderBinding::OpenGLShaderBinding(OpenGLRenderPipelineLayout& owner, std::size_t poolIndex, std::size_t bindingIndex) :
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

	inline const OpenGLRenderPipelineLayout& OpenGLShaderBinding::GetOwner() const
	{
		return m_owner;
	}
}

