// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline auto OpenGLShaderModule::GetExplicitBindings() const -> const std::vector<ExplicitBinding>&
	{
		return m_explicitBindings;
	}
}
