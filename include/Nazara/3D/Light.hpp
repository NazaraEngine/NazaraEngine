// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LIGHT_HPP
#define NAZARA_LIGHT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/3D/Enums.hpp>
#include <Nazara/3D/SceneNode.hpp>
#include <Nazara/Core/Color.hpp>

class NzShader;

class NAZARA_API NzLight : public NzSceneNode
{
	public:
		NzLight(nzLightType type);
		NzLight(const NzLight& light);
		~NzLight();

		void AddToRenderQueue(NzRenderQueue& renderQueue) const;

		void Apply(const NzShader* shader, unsigned int lightUnit) const;

		const NzBoundingBoxf& GetBoundingBox() const;
		NzColor GetAmbientColor() const;
		float GetAttenuation() const;
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

	private:
		void Invalidate();
		void Register();
		void Unregister();
		void UpdateBoundingBox() const;
		bool VisibilityTest(const NzFrustumf& frustum);

		nzLightType m_type;
		mutable NzBoundingBoxf m_boundingBox;
		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		mutable bool m_boundingBoxUpdated;
		float m_attenuation;
		float m_innerAngle;
		float m_outerAngle;
		float m_radius;
};

#endif // NAZARA_LIGHT_HPP
