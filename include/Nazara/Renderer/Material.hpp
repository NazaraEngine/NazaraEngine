// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Texture.hpp>

struct NAZARA_API NzMaterialParams
{
	bool loadDiffuseMap = true;
	bool loadSpecularMap = true;

	bool IsValid() const;
};

class NzMaterial;

using NzMaterialLoader = NzResourceLoader<NzMaterial, NzMaterialParams>;

class NAZARA_API NzMaterial : public NzResource
{
	friend NzMaterialLoader;

	public:
		NzMaterial();
		~NzMaterial();

		void EnableAlphaBlending(bool alphaBlending);
		void EnableZTest(bool zTest);
		void EnableZWrite(bool zWrite);

		NzColor GetAmbientColor() const;
		NzColor GetDiffuseColor() const;
		const NzTexture* GetDiffuseMap() const;
		nzBlendFunc GetDstAlpha() const;
		nzFaceCulling GetFaceCulling() const;
		nzFaceFilling GetFaceFilling() const;
		float GetShininess() const;
		NzColor GetSpecularColor() const;
		const NzTexture* GetSpecularMap() const;
		nzBlendFunc GetSrcAlpha() const;
		nzRendererComparison GetZTestCompare() const;

		bool IsAlphaBlendingEnabled() const;
		bool IsZTestEnabled() const;
		bool IsZWriteEnabled() const;

		void Reset();

		void SetAmbientColor(const NzColor& ambient);
		void SetDiffuseColor(const NzColor& diffuse);
		void SetDiffuseMap(const NzTexture* map);
		void SetDstAlpha(nzBlendFunc func);
		void SetFaceCulling(nzFaceCulling culling);
		void SetFaceFilling(nzFaceFilling filling);
		void SetShininess(float shininess);
		void SetSpecularColor(const NzColor& specular);
		void SetSpecularMap(const NzTexture* map);
		void SetSrcAlpha(nzBlendFunc func);
		void SetZTestCompare(nzRendererComparison compareFunc);

	private:
		nzBlendFunc m_dstAlpha;
		nzBlendFunc m_srcAlpha;
		nzFaceCulling m_faceCulling;
		nzFaceFilling m_faceFilling;
		nzRendererComparison m_zTestCompareFunc;
		NzColor m_ambientColor;
		NzColor m_diffuseColor;
		NzColor m_specularColor;
		const NzTexture* m_diffuseMap;
		const NzTexture* m_specularMap;
		bool m_alphaBlendingEnabled;
		bool m_zTestEnabled;
		bool m_zWriteEnabled;
		float m_shininess;

		static NzMaterialLoader::LoaderList s_loaders;
};

#endif // NAZARA_MATERIAL_HPP
