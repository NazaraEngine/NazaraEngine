// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderInstance.hpp>
#include <algorithm>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	UberShaderInstance::UberShaderInstance(const Shader* shader) :
	m_shader(shader)
	{
	}

	UberShaderInstance::~UberShaderInstance() = default;

	const Shader* UberShaderInstance::GetShader() const
	{
		return m_shader;
	}
}
