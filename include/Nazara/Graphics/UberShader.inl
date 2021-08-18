// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline ShaderStageTypeFlags UberShader::GetSupportedStages() const
	{
		return m_shaderStages;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
