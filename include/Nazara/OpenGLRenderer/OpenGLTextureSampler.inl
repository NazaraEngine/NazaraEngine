// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Sampler& OpenGLTextureSampler::GetSampler(bool withMipmaps) const
	{
		return (withMipmaps) ? m_samplerWithMipmaps : m_samplerWithoutMipmaps;
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
