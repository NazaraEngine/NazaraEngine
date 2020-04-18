// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	auto OpenGLUploadPool::Allocate(UInt64 size) -> Allocation&
	{
		/*const auto& deviceProperties = m_device.GetPhysicalDeviceInfo().properties;
		UInt64 preferredAlignement = deviceProperties.limits.optimalBufferCopyOffsetAlignment;*/

		return Allocate(size, 0); //< FIXME
	}

	auto OpenGLUploadPool::Allocate(UInt64 size, UInt64 alignment) -> Allocation&
	{
		assert(size <= m_blockSize);

		static Allocation dummy;
		return dummy;
	}

	void OpenGLUploadPool::Reset()
	{
		/*for (Block& block : m_blocks)
			block.freeOffset = 0;*/
	}
}
