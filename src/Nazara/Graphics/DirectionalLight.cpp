// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/DirectionalLightShadowData.hpp>

namespace Nz
{
	float DirectionalLight::ComputeContributionScore(const Frustumf& /*viewerFrustum*/) const
	{
		return -std::numeric_limits<float>::infinity();
	}

	bool DirectionalLight::FrustumCull(const Frustumf& /*viewerFrustum*/) const
	{
		return true; //< always visible
	}

	std::unique_ptr<LightShadowData> DirectionalLight::InstanciateShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry) const
	{
		return std::make_unique<DirectionalLightShadowData>(pipeline, elementRegistry, *this, 4);
	}

	void DirectionalLight::UpdateTransform(const Vector3f& /*position*/, const Quaternionf& rotation, const Vector3f& /*scale*/)
	{
		UpdateRotation(rotation);
	}
}
