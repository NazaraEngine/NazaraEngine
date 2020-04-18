// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Context& OpenGLDevice::GetReferenceContext() const
	{
		return *m_referenceContext;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
