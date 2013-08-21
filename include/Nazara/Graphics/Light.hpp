// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LIGHT_HPP
#define NAZARA_LIGHT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/SceneNode.hpp>

class NzShaderProgram;

class NAZARA_API NzLight : public NzSceneNode
{
	public:
		NzLight(nzLightType type);
		NzLight(const NzLight& light);
		~NzLight() = default;

		void AddToRenderQueue(NzAbstractRenderQueue* renderQueue) const override;

		void Enable(const NzShaderProgram* program, unsigned int lightUnit) const;

		NzColor GetAmbientColor() const;
		float GetAttenuation() const;
		const NzBoundingVolumef& GetBoundingVolume() const;
		NzColor GetDiffuseColor() const;
		float GetInnerAngle() const;
		nzLightType GetLightType() const;
		float GetOuterAngle() const;
		float GetRadius() const;
		nzSceneNodeType GetSceneNodeType() const;
		NzColor GetSpecularColor() const;

		void SetAmbientColor(const NzColor& ambient);
		void SetAttenuation(float attenuation);
		void SetDiffuseColor(const NzColor& diffuse);
		void SetInnerAngle(float innerAngle);
		void SetOuterAngle(float outerAngle);
		void SetRadius(float radius);
		void SetSpecularColor(const NzColor& specular);

		NzLight& operator=(const NzLight& light);

		static void Disable(const NzShaderProgram* program, unsigned int lightUnit);

	private:
		bool FrustumCull(const NzFrustumf& frustum) override;
		void Invalidate() override;
		void Register() override;
		void Unregister() override;
		void UpdateBoundingVolume() const;

		nzLightType m_type;
		mutable NzBoundingVolumef m_boundingVolume;
		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		mutable bool m_boundingVolumeUpdated;
		float m_attenuation;
		float m_innerAngle;
		float m_outerAngle;
		float m_radius;
};

#endif // NAZARA_LIGHT_HPP
