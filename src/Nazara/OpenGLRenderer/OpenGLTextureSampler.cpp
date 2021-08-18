// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLTextureSampler::OpenGLTextureSampler(OpenGLDevice& device, TextureSamplerInfo samplerInfo)
	{
		ValidateSamplerInfo(device, samplerInfo);

		BuildSampler(device, m_samplerWithMipmaps, samplerInfo, true);
		BuildSampler(device, m_samplerWithoutMipmaps, samplerInfo, false);
	}

	void OpenGLTextureSampler::BuildSampler(OpenGLDevice& device, GL::Sampler& sampler, const TextureSamplerInfo& samplerInfo, bool withMipmaps)
	{
		if (!sampler.Create(device))
			throw std::runtime_error("failed to create sampler object");

		// In OpenGL, min and mipmap sampler are part of the same enum (and mipmaps filter should only be used with mipmaps)
		if (withMipmaps)
			sampler.SetParameteri(GL_TEXTURE_MIN_FILTER, ToOpenGL(samplerInfo.minFilter, samplerInfo.mipmapMode));
		else
			sampler.SetParameteri(GL_TEXTURE_MIN_FILTER, ToOpenGL(samplerInfo.minFilter));

		sampler.SetParameteri(GL_TEXTURE_MAG_FILTER, ToOpenGL(samplerInfo.magFilter));

		sampler.SetParameteri(GL_TEXTURE_WRAP_S, ToOpenGL(samplerInfo.wrapModeU));
		sampler.SetParameteri(GL_TEXTURE_WRAP_T, ToOpenGL(samplerInfo.wrapModeV));
		sampler.SetParameteri(GL_TEXTURE_WRAP_R, ToOpenGL(samplerInfo.wrapModeW));

		if (samplerInfo.anisotropyLevel > 1.f)
			sampler.SetParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, samplerInfo.anisotropyLevel);
	}
}
