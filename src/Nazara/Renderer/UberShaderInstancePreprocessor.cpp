// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/UberShaderInstancePreprocessor.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	UberShaderInstancePreprocessor::UberShaderInstancePreprocessor(const Shader* shader) :
	UberShaderInstance(shader)
	{
	}

	UberShaderInstancePreprocessor::~UberShaderInstancePreprocessor() = default;

	bool UberShaderInstancePreprocessor::Activate() const
	{
		Renderer::SetShader(m_shader);

		return true;
	}
}
