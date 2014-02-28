// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderInstance.hpp>
#include <algorithm>
#include <Nazara/Renderer/Debug.hpp>

NzUberShaderInstance::NzUberShaderInstance(const NzShader* shader) :
m_shader(shader)
{
}

NzUberShaderInstance::~NzUberShaderInstance() = default;

const NzShader* NzUberShaderInstance::GetShader() const
{
	return m_shader;
}
