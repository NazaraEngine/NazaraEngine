// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LIGHT_HPP
#define NAZARA_LIGHT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Renderable.hpp>

namespace Nz
{
	class Light;
	struct LightUniforms;

	class NAZARA_GRAPHICS_API Light : public Renderable
	{
		public:
			Light(LightType type = LightType_Point);
			Light(const Light& light) = default;
			~Light() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix) const override;

			Light* Clone() const;
			Light* Create() const;

			bool Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const override;

			float GetAmbientFactor() const;
			float GetAttenuation() const;
			Color GetColor() const;
			float GetDiffuseFactor() const;
			float GetInnerAngle() const;
			float GetInnerAngleCosine() const;
			float GetInvRadius() const;
			LightType GetLightType() const;
			float GetOuterAngle() const;
			float GetOuterAngleCosine() const;
			float GetOuterAngleTangent() const;
			float GetRadius() const;

			void SetAmbientFactor(float factor);
			void SetAttenuation(float attenuation);
			void SetColor(const Color& color);
			void SetDiffuseFactor(float factor);
			void SetInnerAngle(float innerAngle);
			void SetLightType(LightType type);
			void SetOuterAngle(float outerAngle);
			void SetRadius(float radius);

			void UpdateBoundingVolume(const Matrix4f& transformMatrix) override;

			Light& operator=(const Light& light) = default;

		private:
			void MakeBoundingVolume() const override;

			LightType m_type;
			Color m_color;
			float m_ambientFactor;
			float m_attenuation;
			float m_diffuseFactor;
			float m_innerAngle;
			float m_innerAngleCosine;
			float m_invRadius;
			float m_outerAngle;
			float m_outerAngleCosine;
			float m_outerAngleTangent;
			float m_radius;
	};

	struct LightUniforms
	{
		struct UniformLocations
		{
			int type;
			int color;
			int factors;
			int parameters1;
			int parameters2;
			int parameters3;
		};

		bool ubo;

		union
		{
			UniformLocations locations;
			int blockLocation;
		};
	};
}

#include <Nazara/Graphics/Light.inl>

#endif // NAZARA_LIGHT_HPP
