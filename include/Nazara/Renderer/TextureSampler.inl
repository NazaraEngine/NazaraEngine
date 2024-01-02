// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/Hash.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline bool TextureSamplerInfo::operator==(const TextureSamplerInfo& samplerInfo) const
	{
		if (!NumberEquals(anisotropyLevel, samplerInfo.anisotropyLevel, 0.99f))
			return false;

		if (magFilter != samplerInfo.magFilter)
			return false;

		if (minFilter != samplerInfo.minFilter)
			return false;

		if (mipmapMode != samplerInfo.mipmapMode)
			return false;

		if (wrapModeU != samplerInfo.wrapModeU)
			return false;

		if (wrapModeV != samplerInfo.wrapModeV)
			return false;

		if (wrapModeW != samplerInfo.wrapModeW)
			return false;

		if (depthCompare != samplerInfo.depthCompare)
			return false;

		if (depthComparison != samplerInfo.depthComparison)
			return false;

		return true;
	}

	inline bool TextureSamplerInfo::operator!=(const TextureSamplerInfo& samplerInfo) const
	{
		return !operator==(samplerInfo);
	}
}

template<>
struct std::hash<Nz::TextureSamplerInfo>
{
	std::size_t operator()(const Nz::TextureSamplerInfo& sampler) const
	{
		return Nz::HashCombine(sampler.anisotropyLevel,
			sampler.magFilter,
			sampler.minFilter,
			sampler.mipmapMode,
			sampler.wrapModeU,
			sampler.wrapModeV,
			sampler.wrapModeW,
			sampler.depthCompare,
			sampler.depthComparison);
	}
};

#include <Nazara/Renderer/DebugOff.hpp>
