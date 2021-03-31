// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const std::vector<GL::Shader>& OpenGLShaderModule::GetShaders() const
	{
		return m_shaders;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
