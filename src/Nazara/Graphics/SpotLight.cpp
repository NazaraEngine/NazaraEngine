// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/SpotLightShadowData.hpp>

namespace Nz
{
	float SpotLight::ComputeContributionScore(const Frustumf& viewerFrustum) const
	{
		// TODO: take luminosity/radius into account
		return viewerFrustum.GetPlane(FrustumPlane::Near).SignedDistance(m_position);
	}

	bool SpotLight::FrustumCull(const Frustumf& viewerFrustum) const
	{
		// We need the radius of the projected circle depending on the distance
		// Tangent = Opposite/Adjacent <=> Opposite = Adjacent * Tangent
		float opposite = m_radius * m_outerAngleTan;

		Vector3f base = Vector3f::Forward() * m_radius;
		Vector3f lExtend = Vector3f::Left() * opposite;
		Vector3f uExtend = Vector3f::Up() * opposite;

		// Test five points against frustum
		std::array<Vector3f, 5> points = {
			m_position,
			base + lExtend + uExtend,
			base + lExtend - uExtend,
			base - lExtend + uExtend,
			base - lExtend - uExtend,
		};

		return viewerFrustum.Contains(points.data(), points.size());
	}

	std::unique_ptr<LightShadowData> SpotLight::InstanciateShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry) const
	{
		return std::make_unique<SpotLightShadowData>(pipeline, elementRegistry, *this);
	}

	void SpotLight::UpdateTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& /*scale*/)
	{
		m_position = position; //< don't call UpdatePosition to prevent double update
		UpdateRotation(rotation);
	}

	void SpotLight::WriteToShader(void* basePtr) const
	{
		AccessByOffset<Vector3f&>(basePtr, PredefinedSpotLightOffsets.colorOffset) = Vector3f(m_color.r, m_color.g, m_color.b) * GetEnergy();
		AccessByOffset<Vector3f&>(basePtr, PredefinedSpotLightOffsets.directionOffset) = m_direction;
		AccessByOffset<Vector3f&>(basePtr, PredefinedSpotLightOffsets.positionOffset) = m_position;
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.ambientFactorOffset) = m_ambientFactor;
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.diffuseFactorOffset) = m_diffuseFactor;
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.innerAngleOffset) = m_innerAngleCos;
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.outerAngleOffset) = m_outerAngleCos;
		AccessByOffset<float&>(basePtr, PredefinedSpotLightOffsets.invRadiusOffset) = m_invRadius;
		AccessByOffset<Matrix4f&>(basePtr, PredefinedSpotLightOffsets.viewProjMatrixOffset) = m_viewProjMatrix;

		//float baseRadius = m_radius * m_outerAngleTan * 1.1f;
		//AccessByOffset<Matrix4f&>(basePtr, PredefinedSpotLightOffsets.worldMatrixOffset) = Matrix4f::Transform(m_position, m_rotation, Vector3f(baseRadius, baseRadius, m_radius));
	}
}
