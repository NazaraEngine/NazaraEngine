// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderInstancePreprocessor.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzUberShaderInstancePreprocessor::NzUberShaderInstancePreprocessor(const NzShader* shader) :
NzUberShaderInstance(shader)
{
}

NzUberShaderInstancePreprocessor::~NzUberShaderInstancePreprocessor() = default;

bool NzUberShaderInstancePreprocessor::Activate() const
{
	NzRenderer::SetShader(m_shader);

	return true;
}
