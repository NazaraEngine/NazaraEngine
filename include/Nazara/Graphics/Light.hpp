// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LIGHT_HPP
#define NAZARA_LIGHT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/SceneNode.hpp>

class NzShader;
struct NzLightUniforms;

class NAZARA_API NzLight : public NzSceneNode
{
	public:
		NzLight(nzLightType type);
		NzLight(const NzLight& light);
		~NzLight() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		void Enable(const NzShader* shader, const NzLightUniforms& uniforms, int offset = 0) const;

		float GetAmbientFactor() const;
		float GetAttenuation() const;
		const NzBoundingVolumef& GetBoundingVolume() const;
		NzColor GetColor() const;
		float GetDiffuseFactor() const;
		float GetInnerAngle() const;
		nzLightType GetLightType() const;
		float GetOuterAngle() const;
		float GetRadius() const;
		nzSceneNodeType GetSceneNodeType() const;

		bool IsDrawable() const;

		void SetAmbientFactor(float factor);
		void SetAttenuation(float attenuation);
		void SetColor(const NzColor& color);
		void SetDiffuseFactor(float factor);
		void SetInnerAngle(float innerAngle);
		void SetLightType(nzLightType type);
		void SetOuterAngle(float outerAngle);
		void SetRadius(float radius);

		NzLight& operator=(const NzLight& light);

		static void Disable(const NzShader* program, const NzLightUniforms& uniforms, int offset = 0);

	private:
		bool FrustumCull(const NzFrustumf& frustum) const override;
		void InvalidateNode() override;
		void Register() override;
		void Unregister() override;
		void UpdateBoundingVolume() const;

		nzLightType m_type;
		mutable NzBoundingVolumef m_boundingVolume;
		NzColor m_color;
		mutable bool m_boundingVolumeUpdated;
		float m_ambientFactor;
		float m_attenuation;
		float m_diffuseFactor;
		float m_innerAngle;
		float m_outerAngle;
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

#endif // NAZARA_LIGHT_HPP
