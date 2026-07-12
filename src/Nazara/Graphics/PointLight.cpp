// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/PointLightShadowData.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>

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

	std::unique_ptr<LightShadowData> PointLight::InstanciateShadowData(FramePipeline& pipeline) const
	{
		return std::make_unique<PointLightShadowData>(pipeline, *this);
	}

	void PointLight::UpdateTransform(const Vector3f& position, const Quaternionf& /*rotation*/, const Vector3f& /*scale*/)
	{
		UpdatePosition(position);
	}

	void PointLight::WriteToShader(void* basePtr, UInt32 shadowIndex) const
	{
		AccessByOffset<Vector3f&>(basePtr, PredefinedPointLightOffsets.colorOffset) = Vector3f(m_color.r, m_color.g, m_color.b) * GetEnergy();
		AccessByOffset<Vector3f&>(basePtr, PredefinedPointLightOffsets.positionOffset) = m_position;
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.ambientFactorOffset) = m_ambientFactor;
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.diffuseFactorOffset) = m_diffuseFactor;
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.radiusOffset) = m_radius;
		AccessByOffset<float&>(basePtr, PredefinedPointLightOffsets.invRadiusOffset) = m_invRadius;
		AccessByOffset<UInt32&>(basePtr, PredefinedPointLightOffsets.shadowIndexOffset) = shadowIndex;
	}
}
