// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline void CommandBufferDeleter::operator()(CommandBuffer* commandBuffer)
	{
		commandBuffer->Release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
