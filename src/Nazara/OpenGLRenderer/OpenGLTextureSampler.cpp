// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLTextureSampler::OpenGLTextureSampler(Vk::Device& device, TextureSamplerInfo samplerInfo)
	{
		VkSamplerCreateInfo createInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		createInfo.magFilter = ToOpenGL(samplerInfo.magFilter);
		createInfo.minFilter = ToOpenGL(samplerInfo.minFilter);
		createInfo.addressModeU = ToOpenGL(samplerInfo.wrapModeU);
		createInfo.addressModeV = ToOpenGL(samplerInfo.wrapModeV);
		createInfo.addressModeW = ToOpenGL(samplerInfo.wrapModeW);
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.mipmapMode = ToOpenGL(samplerInfo.mipmapMode);

		if (samplerInfo.anisotropyLevel > 0.f)
		{
			createInfo.anisotropyEnable = VK_TRUE;
			createInfo.maxAnisotropy = samplerInfo.anisotropyLevel;
		}

		if (!m_sampler.Create(device, createInfo))
			throw std::runtime_error("Failed to create sampler: " + TranslateOpenGLError(m_sampler.GetLastErrorCode()));
	}
}

#endif
