// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLUploadPool::OpenGLUploadPool(Vk::Device& device, UInt64 blockSize) :
	m_blockSize(blockSize),
	m_device(device)
	{
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
