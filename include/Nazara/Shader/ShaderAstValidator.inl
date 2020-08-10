// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstValidator.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	ShaderAstValidator::ShaderAstValidator(const ShaderAst& shader) :
	m_shader(shader)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
