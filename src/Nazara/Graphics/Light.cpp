// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <cstring>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Utilisation des UBOs

NzLight::NzLight(nzLightType type) :
m_type(type),
m_color(NzColor::White),
m_boundingVolumeUpdated(false),
m_ambientFactor((type == nzLightType_Directional) ? 0.2f : 0.f),
m_attenuation(0.9f),
m_diffuseFactor(1.f),
m_innerAngle(15.f),
m_outerAngle(45.f),
m_radius(5.f)
{
}

NzLight::NzLight(const NzLight& light) :
NzSceneNode(light),
m_type(light.m_type),
m_boundingVolume(light.m_boundingVolume),
m_color(light.m_color),
m_boundingVolumeUpdated(light.m_boundingVolumeUpdated),
m_ambientFactor(light.m_ambientFactor),
m_attenuation(light.m_attenuation),
m_diffuseFactor(light.m_diffuseFactor),
m_innerAngle(light.m_innerAngle),
m_outerAngle(light.m_outerAngle),
m_radius(light.m_radius)
{
	SetParent(light);
}

void NzLight::AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const
{
	renderQueue->AddLight(this);
}

void NzLight::Enable(const NzShader* shader, const NzLightUniforms& uniforms, int offset) const
{
	/*
	struct Light
	{
		int type;
		vec4 color;
		vec2 factors;

		vec4 parameters1;
		vec4 parameters2;
		vec2 parameters3;
	};

	Directional
	-P1: vec3 direction

	Point
	-P1: vec3 position + float attenuation
	-P2: vec3 NON-USED + float invRadius

	Spot
	-P1: vec3 position + float attenuation
	-P2: vec3 direction + float invRadius
	-P3: float cosInnerAngle + float cosOuterAngle
	*/

	shader->SendInteger(uniforms.locations.type + offset, m_type);
	shader->SendColor(uniforms.locations.color + offset, m_color);
	shader->SendVector(uniforms.locations.factors + offset, NzVector2f(m_ambientFactor, m_diffuseFactor));

	if (!m_derivedUpdated)
		UpdateDerived();

	switch (m_type)
	{
		case nzLightType_Directional:
			shader->SendVector(uniforms.locations.parameters1 + offset, NzVector4f(m_derivedRotation * NzVector3f::Forward()));
			break;

		case nzLightType_Point:
			shader->SendVector(uniforms.locations.parameters1 + offset, NzVector4f(m_derivedPosition, m_attenuation));
			shader->SendVector(uniforms.locations.parameters2 + offset, NzVector4f(0.f, 0.f, 0.f, 1.f/m_radius));
			break;

		case nzLightType_Spot:
			shader->SendVector(uniforms.locations.parameters1 + offset, NzVector4f(m_derivedPosition, m_attenuation));
			shader->SendVector(uniforms.locations.parameters2 + offset, NzVector4f(m_derivedRotation * NzVector3f::Forward(), 1.f/m_radius));
			shader->SendVector(uniforms.locations.parameters3 + offset, NzVector2f(std::cos(NzDegreeToRadian(m_innerAngle)), std::cos(NzDegreeToRadian(m_outerAngle))));
			break;
	}
}

float NzLight::GetAmbientFactor() const
{
	return m_ambientFactor;
}

float NzLight::GetAttenuation() const
{
	return m_attenuation;
}

const NzBoundingVolumef& NzLight::GetBoundingVolume() const
{
	if (!m_boundingVolumeUpdated)
		UpdateBoundingVolume();

	return m_boundingVolume;
}

NzColor NzLight::GetColor() const
{
	return m_color;
}

float NzLight::GetDiffuseFactor() const
{
	return m_diffuseFactor;
}

float NzLight::GetInnerAngle() const
{
	return m_innerAngle;
}

nzLightType NzLight::GetLightType() const
{
	return m_type;
}

float NzLight::GetOuterAngle() const
{
	return m_outerAngle;
}

float NzLight::GetRadius() const
{
	return m_radius;
}

nzSceneNodeType NzLight::GetSceneNodeType() const
{
	return nzSceneNodeType_Light;
}

bool NzLight::IsDrawable() const
{
	return true;
}

void NzLight::SetAmbientFactor(float factor)
{
	m_ambientFactor = factor;
}

void NzLight::SetAttenuation(float attenuation)
{
	m_attenuation = attenuation;
}

void NzLight::SetColor(const NzColor& color)
{
	m_color = color;
}

void NzLight::SetDiffuseFactor(float factor)
{
	m_diffuseFactor = factor;
}

void NzLight::SetInnerAngle(float innerAngle)
{
	m_innerAngle = innerAngle;
}

void NzLight::SetLightType(nzLightType type)
{
	m_type = type;
}

void NzLight::SetOuterAngle(float outerAngle)
{
	m_outerAngle = outerAngle;

	m_boundingVolume.MakeNull();
	m_boundingVolumeUpdated = false;
}

void NzLight::SetRadius(float radius)
{
	m_radius = radius;

	m_boundingVolume.MakeNull();
	m_boundingVolumeUpdated = false;
}

NzLight& NzLight::operator=(const NzLight& light)
{
	NzSceneNode::operator=(light);

	m_ambientFactor = light.m_ambientFactor;
	m_attenuation = light.m_attenuation;
	m_boundingVolume = light.m_boundingVolume;
	m_boundingVolumeUpdated = light.m_boundingVolumeUpdated;
	m_color = light.m_color;
	m_diffuseFactor = light.m_diffuseFactor;
	m_innerAngle = light.m_innerAngle;
	m_outerAngle = light.m_outerAngle;
	m_radius = light.m_radius;
	m_type = light.m_type;

	return *this;
}

void NzLight::Disable(const NzShader* shader, const NzLightUniforms& uniforms, int offset)
{
	shader->SendInteger(uniforms.locations.type + offset, -1);
}

bool NzLight::FrustumCull(const NzFrustumf& frustum) const
{
	switch (m_type)
	{
		case nzLightType_Directional:
			return true; // Toujours visible

		case nzLightType_Point:
			if (!m_derivedUpdated)
				UpdateDerived();

			// Un test sphérique est bien plus rapide et précis que celui de la bounding box
			return frustum.Contains(NzSpheref(m_derivedPosition, m_radius));

		case nzLightType_Spot:
			return NzSceneNode::FrustumCull(frustum);
	}

	NazaraError("Invalid light type (0x" + NzString::Number(m_type, 16) + ')');
	return false;
}

void NzLight::InvalidateNode()
{
	NzSceneNode::InvalidateNode();

	m_boundingVolumeUpdated = false;
}

void NzLight::Register()
{
}

void NzLight::Unregister()
{
}

void NzLight::UpdateBoundingVolume() const
{
	if (m_boundingVolume.IsNull())
	{
		switch (m_type)
		{
			case nzLightType_Directional:
				m_boundingVolume.MakeInfinite();
				m_boundingVolumeUpdated = true;
				return; // Rien d'autre à faire

			case nzLightType_Point:
			{
				NzVector3f radius(m_radius);
				m_boundingVolume.Set(-radius, radius);
				break;
			}

			case nzLightType_Spot:
			{
				// On forme une boite sur l'origine
				NzBoxf box(NzVector3f::Zero());

				// On calcule le reste des points
				NzVector3f base(NzVector3f::Forward()*m_radius);

				// Il nous faut maintenant le rayon du cercle projeté à cette distance
				// Tangente = Opposé/Adjaçent <=> Opposé = Adjaçent*Tangente
				float radius = m_radius*std::tan(NzDegreeToRadian(m_outerAngle));
				NzVector3f lExtend = NzVector3f::Left()*radius;
				NzVector3f uExtend = NzVector3f::Up()*radius;

				// Et on ajoute ensuite les quatres extrémités de la pyramide
				box.ExtendTo(base + lExtend + uExtend);
				box.ExtendTo(base + lExtend - uExtend);
				box.ExtendTo(base - lExtend + uExtend);
				box.ExtendTo(base - lExtend - uExtend);

				m_boundingVolume.Set(box);
				break;
			}
		}
	}

	switch (m_type)
	{
		case nzLightType_Directional:
			break;

		case nzLightType_Point:
			if (!m_derivedUpdated)
				UpdateDerived();

			m_boundingVolume.Update(NzMatrix4f::Translate(m_derivedPosition)); // Notre BoundingBox ne changera que selon la position
			break;

		case nzLightType_Spot:
			if (!m_derivedUpdated)
				UpdateDerived();

			m_boundingVolume.Update(NzMatrix4f::Transform(m_derivedPosition, m_derivedRotation));
			break;
	}

	m_boundingVolumeUpdated = true;
}
