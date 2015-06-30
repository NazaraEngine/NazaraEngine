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
#include <Nazara/Renderer/RenderTexture.hpp>
#include <Nazara/Renderer/Texture.hpp>

class NzLight;
struct NzLightUniforms;

class NAZARA_GRAPHICS_API NzLight : public NzRenderable
{
	public:
		NzLight(nzLightType type = nzLightType_Point);
		inline NzLight(const NzLight& light);
		NzLight(NzLight&& light) = default;
		~NzLight() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue, const NzMatrix4f& transformMatrix) const override;

		NzLight* Clone() const;
		NzLight* Create() const;

		bool Cull(const NzFrustumf& frustum, const NzMatrix4f& transformMatrix) const override;

		inline void EnableShadowCasting(bool castShadows);

		inline void EnsureShadowMapUpdate() const;

		inline float GetAmbientFactor() const;
		inline float GetAttenuation() const;
		inline NzColor GetColor() const;
		inline float GetDiffuseFactor() const;
		inline float GetInnerAngle() const;
		inline float GetInnerAngleCosine() const;
		inline float GetInvRadius() const;
		inline nzLightType GetLightType() const;
		inline float GetOuterAngle() const;
		inline float GetOuterAngleCosine() const;
		inline float GetOuterAngleTangent() const;
		inline float GetRadius() const;
		inline NzTextureRef GetShadowMap() const;

		inline bool IsShadowCastingEnabled() const;

		inline void SetAmbientFactor(float factor);
		inline void SetAttenuation(float attenuation);
		inline void SetColor(const NzColor& color);
		inline void SetDiffuseFactor(float factor);
		inline void SetInnerAngle(float innerAngle);
		inline void SetLightType(nzLightType type);
		inline void SetOuterAngle(float outerAngle);
		inline void SetRadius(float radius);

		void UpdateBoundingVolume(const NzMatrix4f& transformMatrix) override;

		NzLight& operator=(const NzLight& light);
		NzLight& operator=(NzLight&& light) = default;

	private:
		void MakeBoundingVolume() const override;
		void UpdateShadowMap() const;

		nzLightType m_type;
		NzColor m_color;
		mutable NzTextureRef m_shadowMap;
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

struct NzLightUniforms
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

#include <Nazara/Graphics/Light.inl>

#endif // NAZARA_LIGHT_HPP
