// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void ForwardPipelinePass::InvalidateCommandBuffers()
	{
		m_rebuildCommandBuffer = true;
	}

	inline void ForwardPipelinePass::InvalidateElements()
	{
		m_rebuildElements = true;
	}

	inline std::size_t ForwardPipelinePass::LightKeyHasher::operator()(const LightKey& lightKey) const
	{
		std::size_t lightHash = 5;
		auto CombineHash = [](std::size_t currentHash, std::size_t newHash)
		{
			return currentHash * 23 + newHash;
		};

		std::hash<const Light*> lightPtrHasher;
		for (std::size_t i = 0; i < lightKey.size(); ++i)
			lightHash = CombineHash(lightHash, lightPtrHasher(lightKey[i]));

		return lightHash;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
