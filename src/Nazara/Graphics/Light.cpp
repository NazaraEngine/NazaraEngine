// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Graphics/AbstractRenderQueue.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <cstring>
#include <Nazara/Graphics/Debug.hpp>

///TODO: Use of UBOs
///TODO: Scale ?

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::Light
	* \brief Graphics class that represents a light
	*/

	/*!
	* \brief Constructs a Light object with a type
	*
	* \param type Type of the light
	*/

	Light::Light(LightType type) :
	m_type(type),
	m_shadowMapFormat(PixelFormatType_Depth16),
	m_shadowMapSize(512, 512),
	m_shadowCastingEnabled(false),
	m_shadowMapUpdated(false)
	{
		SetAmbientFactor((type == LightType_Directional) ? 0.2f : 0.f);
		SetAttenuation(0.9f);
		SetColor(Color::White);
		SetDiffuseFactor(1.f);
		SetInnerAngle(15.f);
		SetOuterAngle(45.f);
		SetRadius(5.f);
	}

	/*!
	* \brief Adds this light to the render queue
	*
	* \param renderQueue Queue to be added
	* \param transformMatrix Matrix transformation for this light
	*
	* \remark Produces a NazaraError if type is invalid
	*/

	void Light::AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix) const
	{
		static Matrix4f biasMatrix(0.5f, 0.f, 0.f, 0.f,
		                           0.f, 0.5f, 0.f, 0.f,
								   0.f, 0.f, 0.5f, 0.f,
								   0.5f, 0.5f, 0.5f, 1.f);

		switch (m_type)
		{
			case LightType_Directional:
			{
				AbstractRenderQueue::DirectionalLight light;
				light.ambientFactor = m_ambientFactor;
				light.color = m_color;
				light.diffuseFactor = m_diffuseFactor;
				light.direction = transformMatrix.Transform(Vector3f::Forward(), 0.f);
				light.shadowMap = m_shadowMap.Get();
				light.transformMatrix = Matrix4f::ViewMatrix(transformMatrix.GetRotation() * Vector3f::Forward() * 100.f, transformMatrix.GetRotation()) * Matrix4f::Ortho(0.f, 100.f, 100.f, 0.f, 1.f, 100.f) * biasMatrix;

				renderQueue->AddDirectionalLight(light);
				break;
			}

			case LightType_Point:
			{
				AbstractRenderQueue::PointLight light;
				light.ambientFactor = m_ambientFactor;
				light.attenuation = m_attenuation;
				light.color = m_color;
				light.diffuseFactor = m_diffuseFactor;
				light.invRadius = m_invRadius;
				light.position = transformMatrix.GetTranslation();
				light.radius = m_radius;
				light.shadowMap = m_shadowMap.Get();

				renderQueue->AddPointLight(light);
				break;
			}

			case LightType_Spot:
			{
				AbstractRenderQueue::SpotLight light;
				light.ambientFactor = m_ambientFactor;
				light.attenuation = m_attenuation;
				light.color = m_color;
				light.diffuseFactor = m_diffuseFactor;
				light.direction = transformMatrix.Transform(Vector3f::Forward(), 0.f);
				light.innerAngleCosine = m_innerAngleCosine;
				light.invRadius = m_invRadius;
				light.outerAngleCosine = m_outerAngleCosine;
				light.outerAngleTangent = m_outerAngleTangent;
				light.position = transformMatrix.GetTranslation();
				light.radius = m_radius;
				light.shadowMap = m_shadowMap.Get();
				light.transformMatrix = Matrix4f::ViewMatrix(transformMatrix.GetTranslation(), transformMatrix.GetRotation()) * Matrix4f::Perspective(m_outerAngle*2.f, 1.f, 0.1f, m_radius) * biasMatrix;

				renderQueue->AddSpotLight(light);
				break;
			}

			default:
				NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
				break;
		}
	}

	/*!
	* \brief Clones this light
	* \return Pointer to newly allocated Light
	*/

	Light* Light::Clone() const
	{
		return new Light(*this);
	}

	/*!
	* \brief Creates a default light
	* \return Pointer to newly allocated light
	*/

	Light* Light::Create() const
	{
		return new Light;
	}

	/*!
	* \brief Culls the light if not in the frustum
	* \return true If light is in the frustum
	*
	* \param frustum Symbolizing the field of view
	* \param transformMatrix Matrix transformation for our object
	*
	* \remark Produces a NazaraError if type is invalid
	*/

	bool Light::Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const
	{
		switch (m_type)
		{
			case LightType_Directional:
				return true; // Always visible

			case LightType_Point:
				return frustum.Contains(Spheref(transformMatrix.GetTranslation(), m_radius)); // A sphere test is much faster (and precise)

			case LightType_Spot:
				return frustum.Contains(m_boundingVolume);
		}

		NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
		return false;
	}

	/*!
	* \brief Updates the bounding volume by a matrix
	*
	* \param transformMatrix Matrix transformation for our bounding volume
	*
	* \remark Produces a NazaraError if type is invalid
	*/

	void Light::UpdateBoundingVolume(const Matrix4f& transformMatrix)
	{
		switch (m_type)
		{
			case LightType_Directional:
				break; // Nothing to do (bounding volume should be infinite)

			case LightType_Point:
				m_boundingVolume.Update(transformMatrix.GetTranslation()); // The bounding volume only needs to be shifted
				break;

			case LightType_Spot:
				m_boundingVolume.Update(transformMatrix);
				break;

			default:
				NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
				break;
		}
	}

	/*
	* \brief Makes the bounding volume of this light
	*
	* \remark Produces a NazaraError if type is invalid
	*/

	void Light::MakeBoundingVolume() const
	{
		switch (m_type)
		{
			case LightType_Directional:
				m_boundingVolume.MakeInfinite();
				break;

			case LightType_Point:
			{
				Vector3f radius(m_radius * float(M_SQRT3));
				m_boundingVolume.Set(-radius, radius);
				break;
			}

			case LightType_Spot:
			{
				// We make a box center in the origin
				Boxf box(Vector3f::Zero());

				// We compute the other points
				Vector3f base(Vector3f::Forward() * m_radius);

				// Now we need the radius of the projected circle depending on the distance
				// Tangent = Opposite/Adjacent <=> Opposite = Adjacent * Tangent
				float radius = m_radius * m_outerAngleTangent;
				Vector3f lExtend = Vector3f::Left() * radius;
				Vector3f uExtend = Vector3f::Up() * radius;

				// And we add the four extremities of our pyramid
				box.ExtendTo(base + lExtend + uExtend);
				box.ExtendTo(base + lExtend - uExtend);
				box.ExtendTo(base - lExtend + uExtend);
				box.ExtendTo(base - lExtend - uExtend);

				m_boundingVolume.Set(box);
				break;
			}

			default:
				NazaraError("Invalid light type (0x" + String::Number(m_type, 16) + ')');
				break;
			}
	}

	/*!
	* \brief Updates the shadow map
	*/

	void Light::UpdateShadowMap() const
	{
		if (m_shadowCastingEnabled)
		{
			if (!m_shadowMap)
				m_shadowMap = Texture::New();

			m_shadowMap->Create((m_type == LightType_Point) ? ImageType_Cubemap : ImageType_2D, m_shadowMapFormat, m_shadowMapSize.x, m_shadowMapSize.y);
		}
		else
			m_shadowMap.Reset();

		m_shadowMapUpdated = true;
	}
}
