// Copyright (C) 2015 Jérôme Leclercq
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
///TODO: Scale ?

NzLight::NzLight(nzLightType type) :
m_type(type),
m_color(NzColor::White),
m_ambientFactor((type == nzLightType_Directional) ? 0.2f : 0.f),
m_attenuation(0.9f),
m_diffuseFactor(1.f),
m_innerAngle(15.f),
m_outerAngle(45.f),
m_radius(5.f)
{
}

void NzLight::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const
{
	switch (m_type)
	{
		case nzLightType_Directional:
			renderQueue->AddDirectionalLight(m_color, m_ambientFactor, m_diffuseFactor, transformMatrix.Transform(NzVector3f::Forward(), 0.f));
			break;

		case nzLightType_Point:
			renderQueue->AddPointLight(m_color, m_ambientFactor, m_diffuseFactor, transformMatrix.GetTranslation(), m_radius, m_attenuation);

		case nzLightType_Spot:
			renderQueue->AddSpotLight(m_color, m_ambientFactor, m_diffuseFactor, transformMatrix.GetTranslation(), transformMatrix.Transform(NzVector3f::Forward(), 0.f), m_radius, m_attenuation, m_innerAngle, m_outerAngle);
			break;

		default:
			NazaraError("Invalid light type (0x" + NzString::Number(m_type, 16) + ')');
			break;
	}
}

NzLight* NzLight::Clone() const
{
	return new NzLight(*this);
}

NzLight* NzLight::Create() const
{
	return new NzLight;
}

bool NzLight::Cull(const NzFrustumf& frustum, const NzBoundingVolumef& volume, const NzMatrix4f& transformMatrix) const
{
	switch (m_type)
	{
		case nzLightType_Directional:
			return true; // Always visible

		case nzLightType_Point:
			return frustum.Contains(NzSpheref(transformMatrix.GetTranslation(), m_radius)); // A sphere test is much faster (and precise)

		case nzLightType_Spot:
			return frustum.Contains(volume);
	}

	NazaraError("Invalid light type (0x" + NzString::Number(m_type, 16) + ')');
	return false;
}

float NzLight::GetAmbientFactor() const
{
	return m_ambientFactor;
}

float NzLight::GetAttenuation() const
{
	return m_attenuation;
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

	InvalidateBoundingVolume();
}

void NzLight::SetRadius(float radius)
{
	m_radius = radius;

	InvalidateBoundingVolume();
}

void NzLight::UpdateBoundingVolume(NzBoundingVolumef* boundingVolume, const NzMatrix4f& transformMatrix) const
{
	NazaraAssert(boundingVolume, "Invalid bounding volume");

	switch (m_type)
	{
		case nzLightType_Directional:
			break; // Nothing to do (bounding volume should be infinite)

		case nzLightType_Point:
			boundingVolume->Update(transformMatrix.GetTranslation()); // The bounding volume only needs to be shifted
			break;

		case nzLightType_Spot:
			boundingVolume->Update(transformMatrix);
			break;

		default:
			NazaraError("Invalid light type (0x" + NzString::Number(m_type, 16) + ')');
			break;
	}
}

void NzLight::MakeBoundingVolume() const
{
	switch (m_type)
	{
		case nzLightType_Directional:
			m_boundingVolume.MakeInfinite();
			break;

		case nzLightType_Point:
		{
			NzVector3f radius(m_radius * M_SQRT3);
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

		default:
			NazaraError("Invalid light type (0x" + NzString::Number(m_type, 16) + ')');
			break;
	}
}
