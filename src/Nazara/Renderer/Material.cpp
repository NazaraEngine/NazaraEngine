// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

bool NzMaterialParams::IsValid() const
{
	return true;
}

NzMaterial::NzMaterial() :
m_diffuseMap(nullptr),
m_specularMap(nullptr)
{
	Reset();
}

NzMaterial::NzMaterial(const NzMaterial& material) :
NzResource()
{
	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();

	if (m_specularMap)
		m_specularMap->AddResourceReference();
}

NzMaterial::NzMaterial(NzMaterial&& material)
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Comme ça nous volons la référence du matériau
	material.m_diffuseMap = nullptr;
	material.m_specularMap = nullptr;
}

NzMaterial::~NzMaterial()
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();
}

void NzMaterial::Apply() const
{
	const NzShader* shader = NzRenderer::GetShader();
	#if NAZARA_RENDERER_SAFE
	if (!shader)
	{
		NazaraError("No shader bound");
		return;
	}
	#endif

	int ambientColorLocation = shader->GetUniformLocation("AmbientColor");
	int diffuseColorLocation = shader->GetUniformLocation("DiffuseColor");
	int shininessLocation = shader->GetUniformLocation("Shininess");
	int specularColorLocation = shader->GetUniformLocation("SpecularColor");

	if (ambientColorLocation != -1)
		shader->SendColor(ambientColorLocation, m_ambientColor);

	if (diffuseColorLocation != -1)
		shader->SendColor(diffuseColorLocation, m_diffuseColor);

	if (m_diffuseMap)
	{
		int diffuseMapLocation = shader->GetUniformLocation("DiffuseMap");
		if (diffuseMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(diffuseMapLocation, m_diffuseMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send diffuse map");
		}
	}

	if (shininessLocation != -1)
		shader->SendFloat(shininessLocation, m_shininess);

	if (specularColorLocation != -1)
		shader->SendColor(ambientColorLocation, m_specularColor);

	if (m_specularMap)
	{
		int specularMapLocation = shader->GetUniformLocation("SpecularMap");
		if (specularMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(specularMapLocation, m_specularMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_specularSampler);
			else
				NazaraWarning("Failed to send diffuse map");
		}
	}

	if (m_alphaBlendingEnabled)
	{
		NzRenderer::Enable(nzRendererParameter_Blend, true);
		NzRenderer::SetBlendFunc(m_srcBlend, m_dstBlend);
	}
	else
		NzRenderer::Enable(nzRendererParameter_Blend, false);

	if (m_zTestEnabled)
	{
		NzRenderer::Enable(nzRendererParameter_DepthTest, true);
		NzRenderer::Enable(nzRendererParameter_DepthWrite, m_zWriteEnabled);
		NzRenderer::SetDepthFunc(m_zTestCompareFunc);
	}
	else
		NzRenderer::Enable(nzRendererParameter_DepthTest, false);
}

void NzMaterial::EnableAlphaBlending(bool alphaBlending)
{
	m_alphaBlendingEnabled = alphaBlending;
}

void NzMaterial::EnableZTest(bool zTest)
{
	m_zTestEnabled = zTest;
}

void NzMaterial::EnableZWrite(bool zWrite)
{
	m_zWriteEnabled = zWrite;
}

NzColor NzMaterial::GetAmbientColor() const
{
	return m_ambientColor;
}

NzColor NzMaterial::GetDiffuseColor() const
{
	return m_diffuseColor;
}

NzTextureSampler& NzMaterial::GetDiffuseSampler()
{
	return m_diffuseSampler;
}

const NzTextureSampler& NzMaterial::GetDiffuseSampler() const
{
	return m_diffuseSampler;
}

const NzTexture* NzMaterial::GetDiffuseMap() const
{
	return m_diffuseMap;
}

nzBlendFunc NzMaterial::GetDstBlend() const
{
	return m_dstBlend;
}

nzFaceCulling NzMaterial::GetFaceCulling() const
{
	return m_faceCulling;
}

nzFaceFilling NzMaterial::GetFaceFilling() const
{
	return m_faceFilling;
}

float NzMaterial::GetShininess() const
{
	return m_shininess;
}

NzColor NzMaterial::GetSpecularColor() const
{
	return m_specularColor;
}

const NzTexture* NzMaterial::GetSpecularMap() const
{
	return m_specularMap;
}

NzTextureSampler& NzMaterial::GetSpecularSampler()
{
	return m_specularSampler;
}

const NzTextureSampler& NzMaterial::GetSpecularSampler() const
{
	return m_specularSampler;
}

nzBlendFunc NzMaterial::GetSrcBlend() const
{
	return m_srcBlend;
}

nzRendererComparison NzMaterial::GetZTestCompare() const
{
	return m_zTestCompareFunc;
}

bool NzMaterial::IsAlphaBlendingEnabled() const
{
	return m_alphaBlendingEnabled;
}

bool NzMaterial::IsZTestEnabled() const
{
	return m_zTestEnabled;
}

bool NzMaterial::IsZWriteEnabled() const
{
	return m_zWriteEnabled;
}

bool NzMaterial::LoadFromFile(const NzString& filePath, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromFile(this, filePath, params);
}

bool NzMaterial::LoadFromMemory(const void* data, std::size_t size, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromMemory(this, data, size, params);
}

bool NzMaterial::LoadFromStream(NzInputStream& stream, const NzMaterialParams& params)
{
	return NzMaterialLoader::LoadFromStream(this, stream, params);
}

void NzMaterial::Reset()
{
	if (m_diffuseMap)
	{
		m_diffuseMap->RemoveResourceReference();
		m_diffuseMap = nullptr;
	}

	if (m_specularMap)
	{
		m_specularMap->RemoveResourceReference();
		m_specularMap = nullptr;
	}

	m_alphaBlendingEnabled = false;
	m_ambientColor = NzColor::Black;
	m_diffuseColor = NzColor::White;
	m_diffuseSampler = NzTextureSampler();
	m_dstBlend = nzBlendFunc_Zero;
	m_faceCulling = nzFaceCulling_Back;
	m_faceFilling = nzFaceFilling_Fill;
	m_shininess = 0;
	m_specularColor = NzColor::White;
	m_specularSampler = NzTextureSampler();
	m_srcBlend = nzBlendFunc_One;
	m_zTestCompareFunc = nzRendererComparison_LessOrEqual;
	m_zTestEnabled = true;
	m_zWriteEnabled = true;
}

void NzMaterial::SetAmbientColor(const NzColor& ambient)
{
	m_ambientColor = ambient;
}

void NzMaterial::SetDiffuseColor(const NzColor& diffuse)
{
	m_diffuseColor = diffuse;
}

void NzMaterial::SetDiffuseMap(const NzTexture* map)
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	m_diffuseMap = map;
	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();
}

void NzMaterial::SetDiffuseSampler(const NzTextureSampler& sampler)
{
	m_diffuseSampler = sampler;
}

void NzMaterial::SetDstBlend(nzBlendFunc func)
{
	m_dstBlend = func;
}

void NzMaterial::SetFaceCulling(nzFaceCulling culling)
{
	m_faceCulling = culling;
}

void NzMaterial::SetFaceFilling(nzFaceFilling filling)
{
	m_faceFilling = filling;
}

void NzMaterial::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void NzMaterial::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

void NzMaterial::SetSpecularMap(const NzTexture* map)
{
	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	m_specularMap = map;
	if (m_specularMap)
		m_specularMap->AddResourceReference();
}

void NzMaterial::SetSpecularSampler(const NzTextureSampler& sampler)
{
	m_specularSampler = sampler;
}

void NzMaterial::SetSrcBlend(nzBlendFunc func)
{
	m_srcBlend = func;
}

void NzMaterial::SetZTestCompare(nzRendererComparison compareFunc)
{
	m_zTestEnabled = compareFunc;
}

NzMaterial& NzMaterial::operator=(const NzMaterial& material)
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();

	if (m_specularMap)
		m_specularMap->AddResourceReference();

	return *this;
}

NzMaterial& NzMaterial::operator=(NzMaterial&& material)
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Comme ça nous volons la référence du matériau
	material.m_diffuseMap = nullptr;
	material.m_specularMap = nullptr;

	return *this;
}

const NzMaterial* NzMaterial::GetDefault()
{
	static NzMaterial defaultMaterial;
	static bool initialized = false;

	if (!initialized)
	{
		defaultMaterial.SetFaceCulling(nzFaceCulling_FrontAndBack);
		defaultMaterial.SetFaceFilling(nzFaceFilling_Line);
		defaultMaterial.SetDiffuseColor(NzColor::White);

		initialized = true;
	}

	return &defaultMaterial;
}

NzMaterialLoader::LoaderList NzMaterial::s_loaders;
