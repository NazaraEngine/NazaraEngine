// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline auto OpenGLShaderModule::GetExplicitBindings() const -> const std::vector<ExplicitBinding>&
	{
		return m_explicitBindings;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
