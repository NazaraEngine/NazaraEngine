// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DirectionalLight.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <limits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	float DirectionalLight::ComputeContributionScore(const BoundingVolumef& /*boundingVolume*/) const
	{
		return -std::numeric_limits<float>::infinity();
	}

	void DirectionalLight::FillLightData(void* data) const
	{
		auto lightOffset = PredefinedLightData::GetOffsets();

		AccessByOffset<UInt32&>(data, lightOffset.lightMemberOffsets.type) = UnderlyingCast(BasicLightType::Directional);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.color) = Vector4f(m_color.r, m_color.g, m_color.b, m_color.a);
		AccessByOffset<Vector2f&>(data, lightOffset.lightMemberOffsets.factor) = Vector2f(m_ambientFactor, m_diffuseFactor);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.parameter1) = Vector4f(m_direction.x, m_direction.y, m_direction.z, 0.f);
		AccessByOffset<Vector2f&>(data, lightOffset.lightMemberOffsets.shadowMapSize) = Vector2f(-1.f, -1.f);
	}

	std::unique_ptr<LightShadowData> DirectionalLight::InstanciateShadowData(FramePipeline& pipeline, ElementRendererRegistry& elementRegistry) const
	{
		return nullptr; //< TODO
	}

	void DirectionalLight::UpdateTransform(const Vector3f& /*position*/, const Quaternionf& rotation, const Vector3f& /*scale*/)
	{
		UpdateRotation(rotation);
	}
}
