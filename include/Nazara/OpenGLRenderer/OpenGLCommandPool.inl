// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline OpenGLCommandPool::OpenGLCommandPool(Vk::Device& device, QueueType queueType)
	{
		UInt32 queueFamilyIndex = device.GetDefaultFamilyIndex(queueType);
		if (queueFamilyIndex == Vk::Device::InvalidQueue)
			throw std::runtime_error("QueueType " + std::to_string(UnderlyingCast(queueType)) + " is not supported");

		if (!m_commandPool.Create(device, queueFamilyIndex))
			throw std::runtime_error("Failed to create command pool: " + TranslateOpenGLError(m_commandPool.GetLastErrorCode()));
	}

	inline OpenGLCommandPool::OpenGLCommandPool(Vk::Device& device, UInt32 queueFamilyIndex)
	{
		if (!m_commandPool.Create(device, queueFamilyIndex))
			throw std::runtime_error("Failed to create command pool: " + TranslateOpenGLError(m_commandPool.GetLastErrorCode()));
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
