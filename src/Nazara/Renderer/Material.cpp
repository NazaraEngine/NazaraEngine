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
m_shader(nullptr),
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
	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();

	if (m_heightMap)
		m_heightMap->AddResourceReference();

	if (m_normalMap)
		m_normalMap->AddResourceReference();

	if (m_shader)
		m_shader->AddResourceReference();

	if (m_specularMap)
		m_specularMap->AddResourceReference();
}

NzMaterial::NzMaterial(NzMaterial&& material)
{
	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Comme ça nous volons la référence du matériau
	material.m_diffuseMap = nullptr;
	material.m_heightMap = nullptr;
	material.m_normalMap = nullptr;
	material.m_shader = nullptr;
	material.m_specularMap = nullptr;
}

NzMaterial::~NzMaterial()
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	if (m_shader)
		m_shader->RemoveResourceReference();

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

void NzMaterial::EnableLighting(bool lighting)
{
	m_lightingEnabled = lighting;

	if (m_autoShader)
		m_shader = nullptr;
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

const NzTexture* NzMaterial::GetHeightMap() const
{
	return m_diffuseMap;
}

const NzTexture* NzMaterial::GetNormalMap() const
{
	return m_diffuseMap;
}

const NzShader* NzMaterial::GetShader() const
{
	if (!m_shader)
		UpdateShader();

	return m_shader;
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

	if (m_shader)
	{
		m_shader->RemoveResourceReference();
		m_shader = nullptr;
	}

	if (m_specularMap)
	{
		m_specularMap->RemoveResourceReference();
		m_specularMap = nullptr;
	}

	m_alphaBlendingEnabled = false;
	m_ambientColor = NzColor(128, 128, 128);
	m_autoShader = true;
	m_diffuseColor = NzColor::White;
	m_diffuseSampler = NzTextureSampler();
	m_dstBlend = nzBlendFunc_Zero;
	m_faceCulling = nzFaceCulling_Back;
	m_faceFilling = nzFaceFilling_Fill;
	m_lightingEnabled = true;
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

void NzMaterial::SetHeightMap(const NzTexture* map)
{
	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	m_heightMap = map;
	if (m_heightMap)
		m_heightMap->AddResourceReference();
}

void NzMaterial::SetNormalMap(const NzTexture* map)
{
	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	m_normalMap = map;
	if (m_normalMap)
		m_normalMap->AddResourceReference();
}

void NzMaterial::SetShader(const NzShader* shader)
{
	if (m_shader)
		m_shader->RemoveResourceReference();

	m_autoShader = (shader == nullptr);
	m_shader = shader;
	if (m_shader)
		m_shader->AddResourceReference();
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

	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Cependant comme nous sommes une entité à part nous devons ajouter les références aux ressources
	if (m_diffuseMap)
		m_diffuseMap->AddResourceReference();

	if (m_heightMap)
		m_heightMap->AddResourceReference();

	if (m_normalMap)
		m_normalMap->AddResourceReference();

	if (m_shader)
		m_shader->AddResourceReference();

	if (m_specularMap)
		m_specularMap->AddResourceReference();

	return *this;
}

NzMaterial& NzMaterial::operator=(NzMaterial&& material)
{
	if (m_diffuseMap)
		m_diffuseMap->RemoveResourceReference();

	if (m_heightMap)
		m_heightMap->RemoveResourceReference();

	if (m_normalMap)
		m_normalMap->RemoveResourceReference();

	if (m_shader)
		m_shader->RemoveResourceReference();

	if (m_specularMap)
		m_specularMap->RemoveResourceReference();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et plus rapide

	// Comme ça nous volons la référence du matériau
	material.m_diffuseMap = nullptr;
	material.m_heightMap = nullptr;
	material.m_normalMap = nullptr;
	material.m_shader = nullptr;
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

void NzMaterial::UpdateShader() const
{
	nzUInt32 shaderFlags = 0;
	if (m_diffuseMap)
		shaderFlags |= nzShaderBuilder_DiffuseMapping;

	if (m_lightingEnabled)
	{
		shaderFlags |= nzShaderBuilder_Lighting;

		if (m_normalMap)
			shaderFlags |= nzShaderBuilder_NormalMapping;

		if (m_specularMap)
			shaderFlags |= nzShaderBuilder_SpecularMapping;
	}

	m_shader = NzShaderBuilder::Get(shaderFlags);
	m_shader->AddResourceReference();
}

NzMaterialLoader::LoaderList NzMaterial::s_loaders;
