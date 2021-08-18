// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Sampler& OpenGLTextureSampler::GetSampler(bool withMipmaps) const
	{
		return (withMipmaps) ? m_samplerWithMipmaps : m_samplerWithoutMipmaps;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
