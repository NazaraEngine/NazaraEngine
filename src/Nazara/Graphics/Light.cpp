// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <cstring>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Utilisation des UBOs
///TODO: Scale ?

NzLight::NzLight(nzLightType type) :
m_type(type)
{
	SetAmbientFactor((type == nzLightType_Directional) ? 0.2f : 0.f);
	SetAttenuation(0.9f);
	SetColor(NzColor::White);
	SetDiffuseFactor(1.f);
	SetInnerAngle(15.f);
	SetOuterAngle(45.f);
	SetRadius(5.f);
}

void NzLight::AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const
{
	switch (m_type)
	{
		case nzLightType_Directional:
		{
			NzAbstractRenderQueue::DirectionalLight light;
			light.ambientFactor = m_ambientFactor;
			light.color = m_color;
			light.diffuseFactor = m_diffuseFactor;
			light.direction = transformMatrix.Transform(NzVector3f::Forward(), 0.f);

			renderQueue->AddDirectionalLight(light);
			break;
		}

		case nzLightType_Point:
		{
			NzAbstractRenderQueue::PointLight light;
			light.ambientFactor = m_ambientFactor;
			light.attenuation = m_attenuation;
			light.color = m_color;
			light.diffuseFactor = m_diffuseFactor;
			light.invRadius = m_invRadius;
			light.position = transformMatrix.GetTranslation();
			light.radius = m_radius;

			renderQueue->AddPointLight(light);
			break;
		}

		case nzLightType_Spot:
		{
			NzAbstractRenderQueue::SpotLight light;
			light.ambientFactor = m_ambientFactor;
			light.attenuation = m_attenuation;
			light.color = m_color;
			light.diffuseFactor = m_diffuseFactor;
			light.direction = transformMatrix.Transform(NzVector3f::Forward(), 0.f);
			light.innerAngleCosine = m_innerAngleCosine;
			light.invRadius = m_invRadius;
			light.outerAngleCosine = m_outerAngleCosine;
			light.outerAngleTangent = m_outerAngleTangent;
			light.position = transformMatrix.GetTranslation();
			light.radius = m_radius;

			renderQueue->AddSpotLight(light);
			break;
		}

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

bool NzLight::Cull(const NzFrustumf& frustum, const NzMatrix4f& transformMatrix) const
{
	switch (m_type)
	{
		case nzLightType_Directional:
			return true; // Always visible

		case nzLightType_Point:
			return frustum.Contains(NzSpheref(transformMatrix.GetTranslation(), m_radius)); // A sphere test is much faster (and precise)

		case nzLightType_Spot:
			return frustum.Contains(m_boundingVolume);
	}

	NazaraError("Invalid light type (0x" + NzString::Number(m_type, 16) + ')');
	return false;
}

void NzLight::UpdateBoundingVolume(const NzMatrix4f& transformMatrix)
{
	switch (m_type)
	{
		case nzLightType_Directional:
			break; // Nothing to do (bounding volume should be infinite)

		case nzLightType_Point:
			m_boundingVolume.Update(transformMatrix.GetTranslation()); // The bounding volume only needs to be shifted
			break;

		case nzLightType_Spot:
			m_boundingVolume.Update(transformMatrix);
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
			NzVector3f radius(m_radius * float(M_SQRT3));
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
			float radius = m_radius * m_outerAngleTangent;
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
