// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLUploadPool::OpenGLUploadPool(UInt64 blockSize) :
	m_blockSize(blockSize),
	m_nextAllocationIndex(0)
	{
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
