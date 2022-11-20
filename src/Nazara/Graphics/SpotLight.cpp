// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/SpotLight.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	float SpotLight::ComputeContributionScore(const BoundingVolumef& boundingVolume) const
	{
		// TODO: take luminosity/radius/direction into account
		return Vector3f::SquaredDistance(m_position, boundingVolume.aabb.GetCenter());
	}

	void SpotLight::FillLightData(void* data) const
	{
		auto lightOffset = PredefinedLightData::GetOffsets();

		AccessByOffset<UInt32&>(data, lightOffset.lightMemberOffsets.type) = UnderlyingCast(BasicLightType::Spot);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.color) = Vector4f(m_color.r, m_color.g, m_color.b, m_color.a);
		AccessByOffset<Vector2f&>(data, lightOffset.lightMemberOffsets.factor) = Vector2f(m_ambientFactor, m_diffuseFactor);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.parameter1) = Vector4f(m_position.x, m_position.y, m_position.z, m_invRadius);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.parameter2) = Vector4f(m_direction.x, m_direction.y, m_direction.z, 0.f);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.parameter3) = Vector4f(m_innerAngleCos, m_outerAngleCos, 0.f, 0.f);
		AccessByOffset<UInt8&>(data, lightOffset.lightMemberOffsets.shadowMappingFlag) = 0;
		AccessByOffset<Matrix4f&>(data, lightOffset.lightMemberOffsets.viewProjMatrix) = m_viewProjMatrix;
	}

	void SpotLight::UpdateTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& /*scale*/)
	{
		m_position = position; //< don't call UpdatePosition to prevent double update
		UpdateRotation(rotation);
	}
}
