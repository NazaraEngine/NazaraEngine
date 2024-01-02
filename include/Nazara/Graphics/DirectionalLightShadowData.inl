// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void DirectionalLightShadowData::EnableShadowStabilization(bool enable)
	{
		m_isShadowStabilizationEnabled = enable;
	}

	inline std::size_t DirectionalLightShadowData::GetCascadeCount() const
	{
		return m_cascadeCount;
	}

	inline void DirectionalLightShadowData::GetCascadeData(const AbstractViewer* viewer, SparsePtr<float> distance, SparsePtr<Matrix4f> viewProjMatrix) const
	{
		assert(viewer);
		PerViewerData& viewerData = *Retrieve(m_viewerData, viewer);

		for (const auto& cascadeData : viewerData.cascades)
		{
			if (distance)
				*distance++ = cascadeData.distance;

			if (viewProjMatrix)
				*viewProjMatrix++ = cascadeData.viewProjMatrix;
		}
	}

	inline bool DirectionalLightShadowData::IsShadowStabilization() const
	{
		return m_isShadowStabilizationEnabled;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
