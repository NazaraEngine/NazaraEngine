// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/LightComponent.hpp>
#include <Nazara/Core/Algorithm.hpp>

namespace Ndk
{
	bool LightComponent::Serialize(Nz::SerializationContext& context) const
	{
		Nz::UInt8 lightType = static_cast<Nz::UInt8>(GetLightType());
		if (!Nz::Serialize(context, lightType))
			return false;

		Nz::Color lightColor = GetColor();
		if (!Nz::Serialize(context, lightColor))
			return false;

		float ambiantFactor = GetAmbientFactor();
		if (!Nz::Serialize(context, ambiantFactor))
			return false;

		float attenuation = GetAttenuation();
		if (!Nz::Serialize(context, attenuation))
			return false;

		float diffuseFactor = GetDiffuseFactor();
		if (!Nz::Serialize(context, diffuseFactor))
			return false;

		float innerAngle = GetInnerAngle();
		if (!Nz::Serialize(context, innerAngle))
			return false;

		float radius = GetRadius();
		if (!Nz::Serialize(context, radius))
			return false;

		float outerAngle = GetOuterAngle();
		if (!Nz::Serialize(context, outerAngle))
			return false;

		return true;
	}

	bool LightComponent::Unserialize(Nz::SerializationContext& context)
	{
		Nz::UInt8 lightType;
		if (!Nz::Unserialize(context, &lightType))
			return false;

		if (lightType > Nz::LightType_Max)
			return false;

		Nz::Color lightColor;
		if (!Nz::Unserialize(context, &lightColor))
			return false;

		float ambiantFactor;
		if (!Nz::Unserialize(context, &ambiantFactor))
			return false;

		float attenuation;
		if (!Nz::Unserialize(context, &attenuation))
			return false;

		float diffuseFactor;
		if (!Nz::Unserialize(context, &diffuseFactor))
			return false;

		float innerAngle;
		if (!Nz::Unserialize(context, &innerAngle))
			return false;

		float radius;
		if (!Nz::Unserialize(context, &radius))
			return false;

		float outerAngle;
		if (!Nz::Unserialize(context, &outerAngle))
			return false;

		SetAmbientFactor(ambiantFactor);
		SetAttenuation(attenuation);
		SetColor(lightColor);
		SetDiffuseFactor(diffuseFactor);
		SetLightType(static_cast<Nz::LightType>(lightType));
		SetInnerAngle(innerAngle);
		SetOuterAngle(outerAngle);
		SetRadius(radius);

		return true;
	}

	ComponentIndex LightComponent::componentIndex;
}
