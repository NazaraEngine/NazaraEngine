// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderValidator.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	ShaderValidator::ShaderValidator(const ShaderAst& shader) :
	m_shader(shader)
	{
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
