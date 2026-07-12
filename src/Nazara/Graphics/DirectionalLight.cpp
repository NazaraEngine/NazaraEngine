// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/DirectionalLightShadowData.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>

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

	std::unique_ptr<LightShadowData> DirectionalLight::InstanciateShadowData(FramePipeline& pipeline) const
	{
		return std::make_unique<DirectionalLightShadowData>(pipeline, *this, 4);
	}

	void DirectionalLight::UpdateTransform(const Vector3f& /*position*/, const Quaternionf& rotation, const Vector3f& /*scale*/)
	{
		UpdateRotation(rotation);
	}

	void DirectionalLight::WriteToShader(void* basePtr, UInt32 shadowIndex) const
	{
		AccessByOffset<Vector3f&>(basePtr, PredefinedDirectionalLightOffsets.colorOffset) = Vector3f(m_color.r, m_color.g, m_color.b) * GetEnergy();
		AccessByOffset<Vector3f&>(basePtr, PredefinedDirectionalLightOffsets.directionOffset) = m_direction;
		AccessByOffset<float&>(basePtr, PredefinedDirectionalLightOffsets.ambientFactorOffset) = m_ambientFactor;
		AccessByOffset<float&>(basePtr, PredefinedDirectionalLightOffsets.diffuseFactorOffset) = m_diffuseFactor;
		AccessByOffset<UInt32&>(basePtr, PredefinedDirectionalLightOffsets.shadowIndexOffset) = shadowIndex;
	}
}
