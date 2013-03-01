// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <cstring>
#include <Nazara/Renderer/Debug.hpp>

bool NzMaterialParams::IsValid() const
{
	return true;
}

NzMaterial::NzMaterial() :
m_customShader(nullptr),
m_diffuseMap(nullptr),
m_heightMap(nullptr),
m_normalMap(nullptr),
m_specularMap(nullptr)
{
	Reset();
}

NzMaterial::NzMaterial(const NzMaterial& material) :
NzResource()
{
	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Cependant comme nous sommes une entité à part nous devons ajouter les références aux ressources
	if (m_customShader)
		m_customShader->AddResourceReference();

	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();

	if (m_heightMap)
		m_heightMap->AddResourceReference();

	if (m_normalMap)
		m_normalMap->AddResourceReference();

	if (m_specularMap)
		m_specularMap->AddResourceReference();
}

NzMaterial::NzMaterial(NzMaterial&& material)
{
	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Nous "volons" la référence du matériau
	material.m_customShader = nullptr;
	material.m_diffuseMap = nullptr;
	material.m_heightMap = nullptr;
	material.m_normalMap = nullptr;
	material.m_specularMap = nullptr;
}

NzMaterial::~NzMaterial()
{
	if (m_customShader)
		m_customShader->RemoveResourceReference();

	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();
}

void NzMaterial::Apply(const NzShader* shader) const
{
	int ambientColorLocation = shader->GetUniformLocation("MaterialAmbient");
	int diffuseColorLocation = shader->GetUniformLocation("MaterialDiffuse");
	int shininessLocation = shader->GetUniformLocation("MaterialShininess");
	int specularColorLocation = shader->GetUniformLocation("MaterialSpecular");

	if (ambientColorLocation != -1)
		shader->SendColor(ambientColorLocation, m_ambientColor);

	if (diffuseColorLocation != -1)
		shader->SendColor(diffuseColorLocation, m_diffuseColor);

	if (m_diffuseMap)
	{
		int diffuseMapLocation = shader->GetUniformLocation("MaterialDiffuseMap");
		if (diffuseMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(diffuseMapLocation, m_diffuseMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send diffuse map");
		}
	}

	if (m_heightMap)
	{
		int heightMapLocation = shader->GetUniformLocation("MaterialHeightMap");
		if (heightMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(heightMapLocation, m_heightMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send height map");
		}
	}

	if (m_normalMap)
	{
		int normalMapLocation = shader->GetUniformLocation("MaterialNormalMap");
		if (normalMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(normalMapLocation, m_normalMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send normal map");
		}
	}

	if (shininessLocation != -1)
		shader->SendFloat(shininessLocation, m_shininess);

	if (specularColorLocation != -1)
		shader->SendColor(specularColorLocation, m_specularColor);

	if (m_specularMap)
	{
		int specularMapLocation = shader->GetUniformLocation("MaterialSpecularMap");
		if (specularMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(specularMapLocation, m_specularMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_specularSampler);
			else
				NazaraWarning("Failed to send specular map");
		}
	}

	if (m_alphaBlendingEnabled)
	{
		NzRenderer::Enable(nzRendererParameter_Blend, true);
		NzRenderer::SetBlendFunc(m_srcBlend, m_dstBlend);
	}
	else
		NzRenderer::Enable(nzRendererParameter_Blend, false);

	if (m_faceCullingEnabled)
	{
		NzRenderer::Enable(nzRendererParameter_FaceCulling, true);
		NzRenderer::SetFaceCulling(m_faceCulling);
	}
	else
		NzRenderer::Enable(nzRendererParameter_FaceCulling, false);

	NzRenderer::SetFaceFilling(m_faceFilling);

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

void NzMaterial::EnableFaceCulling(bool faceCulling)
{
	m_faceCullingEnabled = faceCulling;
}

void NzMaterial::EnableLighting(bool lighting)
{
	m_lightingEnabled = lighting;
	if (m_lightingEnabled)
		m_shaderFlags |= nzShaderFlags_Lighting;
	else
		m_shaderFlags &= ~nzShaderFlags_Lighting;
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

NzTexture* NzMaterial::GetDiffuseMap() const
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

NzTexture* NzMaterial::GetHeightMap() const
{
	return m_diffuseMap;
}

NzTexture* NzMaterial::GetNormalMap() const
{
	return m_diffuseMap;
}

const NzShader* NzMaterial::GetCustomShader() const
{
	return m_customShader;
}

nzUInt32 NzMaterial::GetShaderFlags() const
{
	return m_shaderFlags;
}

float NzMaterial::GetShininess() const
{
	return m_shininess;
}

NzColor NzMaterial::GetSpecularColor() const
{
	return m_specularColor;
}

NzTexture* NzMaterial::GetSpecularMap() const
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

bool NzMaterial::HasCustomShader() const
{
	return m_customShader != nullptr;
}

bool NzMaterial::IsAlphaBlendingEnabled() const
{
	return m_alphaBlendingEnabled;
}

bool NzMaterial::IsFaceCullingEnabled() const
{
	return m_faceCullingEnabled;
}

bool NzMaterial::IsLightingEnabled() const
{
	return m_lightingEnabled;
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
	if (m_customShader)
	{
		m_customShader->RemoveResourceReference();
		m_customShader = nullptr;
	}

	if (m_diffuseMap)
	{
		m_diffuseMap->RemoveResourceReference();
		m_diffuseMap = nullptr;
	}

	if (m_heightMap)
	{
		m_heightMap->RemoveResourceReference();
		m_heightMap = nullptr;
	}

	if (m_normalMap)
	{
		m_normalMap->RemoveResourceReference();
		m_normalMap = nullptr;
	}

	if (m_specularMap)
	{
		m_specularMap->RemoveResourceReference();
		m_specularMap = nullptr;
	}

	m_alphaBlendingEnabled = false;
	m_ambientColor = NzColor(128, 128, 128);
	m_diffuseColor = NzColor::White;
	m_diffuseSampler = NzTextureSampler();
	m_dstBlend = nzBlendFunc_Zero;
	m_faceCulling = nzFaceCulling_Back;
	m_faceCullingEnabled = true;
	m_faceFilling = nzFaceFilling_Fill;
	m_lightingEnabled = true;
	m_shaderFlags = nzShaderFlags_Lighting;
	m_shininess = 50.f;
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

void NzMaterial::SetCustomShader(const NzShader* shader)
{
	if (m_customShader != shader)
	{
		if (m_customShader)
			m_customShader->RemoveResourceReference();

		m_customShader = shader;
		if (m_customShader)
			m_customShader->AddResourceReference();
	}
}

void NzMaterial::SetDiffuseColor(const NzColor& diffuse)
{
	m_diffuseColor = diffuse;
}

void NzMaterial::SetDiffuseMap(NzTexture* map)
{
	if (m_diffuseMap != map)
	{
		if (m_diffuseMap)
		{
			m_diffuseMap->RemoveResourceReference();
			m_shaderFlags &= ~nzShaderFlags_DiffuseMapping;
		}

		m_diffuseMap = map;
		if (m_diffuseMap)
		{
			m_diffuseMap->AddResourceReference();
			m_shaderFlags |= nzShaderFlags_DiffuseMapping;
		}
	}
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

void NzMaterial::SetHeightMap(NzTexture* map)
{
	if (m_heightMap != map)
	{
		if (m_heightMap)
			m_heightMap->RemoveResourceReference();

		m_heightMap = map;
		if (m_heightMap)
			m_heightMap->AddResourceReference();
	}
}

void NzMaterial::SetNormalMap(NzTexture* map)
{
	if (m_normalMap != map)
	{
		if (m_normalMap)
		{
			m_normalMap->RemoveResourceReference();
			m_shaderFlags &= ~nzShaderFlags_NormalMapping;
		}

		m_normalMap = map;
		if (m_normalMap)
		{
			m_normalMap->AddResourceReference();
			m_shaderFlags |= nzShaderFlags_NormalMapping;
		}
	}
}

void NzMaterial::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void NzMaterial::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

void NzMaterial::SetSpecularMap(NzTexture* map)
{
	if (m_specularMap != map)
	{
		if (m_specularMap)
		{
			m_specularMap->RemoveResourceReference();
			m_shaderFlags &= ~nzShaderFlags_SpecularMapping;
		}

		m_specularMap = map;
		if (m_specularMap)
		{
			m_specularMap->AddResourceReference();
			m_shaderFlags |= nzShaderFlags_SpecularMapping;
		}
	}
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
	if (m_customShader)
		m_customShader->RemoveResourceReference();

	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Cependant comme nous sommes une entité à part nous devons ajouter les références aux ressources
	if (m_customShader)
		m_customShader->AddResourceReference();

	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();

	if (m_heightMap)
		m_heightMap->AddResourceReference();

	if (m_normalMap)
		m_normalMap->AddResourceReference();

	if (m_specularMap)
		m_specularMap->AddResourceReference();

	return *this;
}

NzMaterial& NzMaterial::operator=(NzMaterial&& material)
{
	if (m_customShader)
		m_customShader->RemoveResourceReference();

	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Comme ça nous volons la référence du matériau
	material.m_customShader = nullptr;
	material.m_diffuseMap = nullptr;
	material.m_heightMap = nullptr;
	material.m_normalMap = nullptr;
	material.m_specularMap = nullptr;

	return *this;
}

NzMaterial* NzMaterial::GetDefault()
{
	static NzMaterial defaultMaterial;
	static bool initialized = false;

	if (!initialized)
	{
		defaultMaterial.EnableLighting(false);
		defaultMaterial.SetFaceCulling(nzFaceCulling_FrontAndBack);
		defaultMaterial.SetFaceFilling(nzFaceFilling_Line);
		defaultMaterial.SetDiffuseColor(NzColor::White);

		initialized = true;
	}

	return &defaultMaterial;
}

NzMaterialLoader::LoaderList NzMaterial::s_loaders;
