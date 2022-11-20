// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/PointLight.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Graphics/PredefinedShaderStructs.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	float PointLight::ComputeContributionScore(const BoundingVolumef& boundingVolume) const
	{
		// TODO: take luminosity/radius into account
		return Vector3f::SquaredDistance(m_position, boundingVolume.aabb.GetCenter());
	}

	void PointLight::FillLightData(void* data) const
	{
		auto lightOffset = PredefinedLightData::GetOffsets();

		AccessByOffset<UInt32&>(data, lightOffset.lightMemberOffsets.type) = UnderlyingCast(BasicLightType::Point);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.color) = Vector4f(m_color.r, m_color.g, m_color.b, m_color.a);
		AccessByOffset<Vector2f&>(data, lightOffset.lightMemberOffsets.factor) = Vector2f(m_ambientFactor, m_diffuseFactor);
		AccessByOffset<Vector4f&>(data, lightOffset.lightMemberOffsets.parameter1) = Vector4f(m_position.x, m_position.y, m_position.z, m_invRadius);
		AccessByOffset<Vector2f&>(data, lightOffset.lightMemberOffsets.shadowMapSize) = Vector2f(-1.f, -1.f);
	}

	void PointLight::UpdateTransform(const Vector3f& position, const Quaternionf& /*rotation*/, const Vector3f& /*scale*/)
	{
		UpdatePosition(position);
	}
}
