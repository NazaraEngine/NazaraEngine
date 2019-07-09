// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LIGHT_HPP
#define NAZARA_LIGHT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Renderable.hpp>
#include <Nazara/Renderer/Texture.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API Light : public Renderable
	{
		public:
			Light(LightType type = LightType_Point);
			inline Light(const Light& light);
			Light(Light&& light) = default;
			~Light() = default;

			void AddToRenderQueue(AbstractRenderQueue* renderQueue, const Matrix4f& transformMatrix) const override;

			Light* Clone() const;
			Light* Create() const;

			bool Cull(const Frustumf& frustum, const Matrix4f& transformMatrix) const override;

			inline void EnableShadowCasting(bool castShadows);

			inline void EnsureShadowMapUpdate() const;

			inline float GetAmbientFactor() const;
			inline float GetAttenuation() const;
			inline Color GetColor() const;
			inline float GetDiffuseFactor() const;
			inline float GetInnerAngle() const;
			inline float GetInnerAngleCosine() const;
			inline float GetInvRadius() const;
			inline LightType GetLightType() const;
			inline float GetOuterAngle() const;
			inline float GetOuterAngleCosine() const;
			inline float GetOuterAngleTangent() const;
			inline float GetRadius() const;
			inline TextureRef GetShadowMap() const;
			inline PixelFormatType GetShadowMapFormat() const;
			inline const Vector2ui& GetShadowMapSize() const;

			inline bool IsShadowCastingEnabled() const;

			inline void SetAmbientFactor(float factor);
			inline void SetAttenuation(float attenuation);
			inline void SetColor(const Color& color);
			inline void SetDiffuseFactor(float factor);
			inline void SetInnerAngle(float innerAngle);
			inline void SetLightType(LightType type);
			inline void SetOuterAngle(float outerAngle);
			inline void SetRadius(float radius);
			inline void SetShadowMapFormat(PixelFormatType shadowFormat);
			inline void SetShadowMapSize(const Vector2ui& size);

			void UpdateBoundingVolume(const Matrix4f& transformMatrix) override;

			Light& operator=(const Light& light);
			Light& operator=(Light&& light) = default;

		private:
			void MakeBoundingVolume() const override;
			inline void InvalidateShadowMap();
			void UpdateShadowMap() const;

			Color m_color;
			LightType m_type;
			PixelFormatType m_shadowMapFormat;
			Vector2ui m_shadowMapSize;
			mutable TextureRef m_shadowMap;
			bool m_shadowCastingEnabled;
			mutable bool m_shadowMapUpdated;
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
			int lightViewProjMatrix;
			int parameters1;
			int parameters2;
			int parameters3;
			int shadowMapping;
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
