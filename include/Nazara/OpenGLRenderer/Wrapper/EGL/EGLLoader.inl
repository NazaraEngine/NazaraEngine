// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline EGLDisplay EGLLoader::GetDefaultDisplay() const
	{
		return m_defaultDisplay;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
