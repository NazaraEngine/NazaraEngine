// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PointLightShadowData.hpp>

namespace Nz
{
	float PointLight::ComputeContributionScore(const Frustumf& viewerFrustum) const
	{
		// TODO: take luminosity/radius into account
		return viewerFrustum.GetPlane(FrustumPlane::Near).SignedDistance(m_position);
	}

	bool PointLight::FrustumCull(const Frustumf& viewerFrustum) const
	{
		return viewerFrustum.Intersect(Spheref(m_position, m_radius)) != IntersectionSide::Outside;
	}

	std::unique_ptr<LightShadowData> PointLight::InstanciateShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry) const
	{
		return std::make_unique<PointLightShadowData>(pipeline, elementRegistry, *this);
	}

	void PointLight::UpdateTransform(const Vector3f& position, const Quaternionf& /*rotation*/, const Vector3f& /*scale*/)
	{
		UpdatePosition(position);
	}
}
