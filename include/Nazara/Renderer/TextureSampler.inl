// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Math/Algorithm.hpp>
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
		std::size_t seed = 0;
		Nz::HashCombine(seed, sampler.anisotropyLevel);
		Nz::HashCombine(seed, sampler.magFilter);
		Nz::HashCombine(seed, sampler.minFilter);
		Nz::HashCombine(seed, sampler.mipmapMode);
		Nz::HashCombine(seed, sampler.wrapModeU);
		Nz::HashCombine(seed, sampler.wrapModeV);
		Nz::HashCombine(seed, sampler.wrapModeW);
		Nz::HashCombine(seed, sampler.depthCompare);
		Nz::HashCombine(seed, sampler.depthComparison);

		return seed;
	}
};

#include <Nazara/Renderer/DebugOff.hpp>
