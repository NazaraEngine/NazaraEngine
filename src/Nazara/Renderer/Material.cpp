// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Shader.hpp>
#include <Nazara/Renderer/ShaderManager.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

bool NzMaterialParams::IsValid() const
{
	return true;
}

NzMaterial::NzMaterial()
{
	Reset();
}

NzMaterial::NzMaterial(const NzMaterial& material) :
NzResource()
{
	Copy(material);
}

NzMaterial::NzMaterial(NzMaterial&& material)
{
	Copy(material);

	// Nous "volons" la référence du matériau
	material.m_alphaMap.Reset();
	material.m_diffuseMap.Reset();
	material.m_emissiveMap.Reset();
	material.m_heightMap.Reset();
	material.m_normalMap.Reset();
	material.m_specularMap.Reset();

	for (unsigned int i = 0; i <= nzShaderTarget_Max; ++i)
		for (ShaderUnit& unit : material.m_shaders[i])
			unit.shader.Reset();
}

void NzMaterial::Apply(const NzShader* shader) const
{
	int alphaThresholdLocation = shader->GetUniformLocation(nzShaderUniform_MaterialAlphaThreshold);
	int ambientColorLocation = shader->GetUniformLocation(nzShaderUniform_MaterialAmbient);
	int diffuseColorLocation = shader->GetUniformLocation(nzShaderUniform_MaterialDiffuse);
	int shininessLocation = shader->GetUniformLocation(nzShaderUniform_MaterialShininess);
	int specularColorLocation = shader->GetUniformLocation(nzShaderUniform_MaterialSpecular);

	if (m_alphaMap)
	{
		int alphaMapLocation = shader->GetUniformLocation(nzShaderUniform_MaterialAlphaMap);
		if (alphaMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(alphaMapLocation, m_alphaMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send diffuse map");
		}
	}

	if (alphaThresholdLocation != -1)
		shader->SendFloat(alphaThresholdLocation, m_alphaThreshold);

	if (ambientColorLocation != -1)
		shader->SendColor(ambientColorLocation, m_ambientColor);

	if (diffuseColorLocation != -1)
		shader->SendColor(diffuseColorLocation, m_diffuseColor);

	if (m_diffuseMap)
	{
		int diffuseMapLocation = shader->GetUniformLocation(nzShaderUniform_MaterialDiffuseMap);
		if (diffuseMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(diffuseMapLocation, m_diffuseMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send diffuse map");
		}
	}

	if (m_emissiveMap)
	{
		int emissiveMapLocation = shader->GetUniformLocation(nzShaderUniform_MaterialEmissiveMap);
		if (emissiveMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(emissiveMapLocation, m_emissiveMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			else
				NazaraWarning("Failed to send emissive map");
		}
	}

	if (m_heightMap)
	{
		int heightMapLocation = shader->GetUniformLocation(nzShaderUniform_MaterialHeightMap);
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
		int normalMapLocation = shader->GetUniformLocation(nzShaderUniform_MaterialNormalMap);
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
		int specularMapLocation = shader->GetUniformLocation(nzShaderUniform_MaterialSpecularMap);
		if (specularMapLocation != -1)
		{
			nzUInt8 textureUnit;
			if (shader->SendTexture(specularMapLocation, m_specularMap, &textureUnit))
				NzRenderer::SetTextureSampler(textureUnit, m_specularSampler);
			else
				NazaraWarning("Failed to send specular map");
		}
	}

	NzRenderer::SetRenderStates(m_states);
}

void NzMaterial::Enable(nzRendererParameter renderParameter, bool enable)
{
	#ifdef NAZARA_DEBUG
	if (renderParameter > nzRendererParameter_Max)
	{
		NazaraError("Renderer parameter out of enum");
		return;
	}
	#endif

	m_states.parameters[renderParameter] = enable;
}

void NzMaterial::EnableAlphaTest(bool alphaTest)
{
	m_alphaTestEnabled = alphaTest;
}

void NzMaterial::EnableLighting(bool lighting)
{
	m_lightingEnabled = lighting;
}

NzTexture* NzMaterial::GetAlphaMap() const
{
	return m_alphaMap;
}

float NzMaterial::GetAlphaThreshold() const
{
	return m_alphaThreshold;
}

NzColor NzMaterial::GetAmbientColor() const
{
	return m_ambientColor;
}

nzRendererComparison NzMaterial::GetDepthFunc() const
{
	return m_states.depthFunc;
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
	return m_states.dstBlend;
}

NzTexture* NzMaterial::GetEmissiveMap() const
{
	return m_emissiveMap;
}

nzFaceCulling NzMaterial::GetFaceCulling() const
{
	return m_states.faceCulling;
}

nzFaceFilling NzMaterial::GetFaceFilling() const
{
	return m_states.faceFilling;
}

NzTexture* NzMaterial::GetHeightMap() const
{
	return m_heightMap;
}

NzTexture* NzMaterial::GetNormalMap() const
{
	return m_normalMap;
}

const NzRenderStates& NzMaterial::GetRenderStates() const
{
	return m_states;
}

const NzShader* NzMaterial::GetShader(nzShaderTarget target, nzUInt32 flags) const
{
	const ShaderUnit& unit = m_shaders[target][flags];
	if (!unit.shader.IsValid())
		GenerateShader(target, flags);

	return unit.shader;
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
	return m_states.srcBlend;
}

bool NzMaterial::HasAlphaMap() const
{
	return m_alphaMap.IsValid();
}

bool NzMaterial::HasDiffuseMap() const
{
	return m_diffuseMap.IsValid();
}

bool NzMaterial::HasEmissiveMap() const
{
	return m_emissiveMap.IsValid();
}

bool NzMaterial::HasHeightMap() const
{
	return m_heightMap.IsValid();
}

bool NzMaterial::HasNormalMap() const
{
	return m_normalMap.IsValid();
}

bool NzMaterial::HasSpecularMap() const
{
	return m_specularMap.IsValid();
}

bool NzMaterial::IsAlphaTestEnabled() const
{
	return m_alphaTestEnabled;
}

bool NzMaterial::IsEnabled(nzRendererParameter parameter) const
{
	#ifdef NAZARA_DEBUG
	if (parameter > nzRendererParameter_Max)
	{
		NazaraError("Renderer parameter out of enum");
		return false;
	}
	#endif

	return m_states.parameters[parameter];
}

bool NzMaterial::IsLightingEnabled() const
{
	return m_lightingEnabled;
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
	m_alphaMap.Reset();
	m_diffuseMap.Reset();
	m_emissiveMap.Reset();
	m_heightMap.Reset();
	m_normalMap.Reset();
	m_specularMap.Reset();

	for (unsigned int i = 0; i <= nzShaderTarget_Max; ++i)
	{
		for (ShaderUnit& unit : m_shaders[i])
		{
			unit.custom = false;
			unit.shader.Reset();
		}
	}

	m_alphaThreshold = 0.2f;
	m_alphaTestEnabled = false;
	m_ambientColor = NzColor(128, 128, 128);
	m_diffuseColor = NzColor::White;
	m_diffuseSampler = NzTextureSampler();
	m_lightingEnabled = true;
	m_shininess = 50.f;
	m_specularColor = NzColor::White;
	m_specularSampler = NzTextureSampler();
	m_states = NzRenderStates();
	m_states.parameters[nzRendererParameter_DepthBuffer] = true;
	m_states.parameters[nzRendererParameter_FaceCulling] = true;
}

bool NzMaterial::SetAlphaMap(const NzString& texturePath)
{
	std::unique_ptr<NzTexture> texture(new NzTexture);
	if (!texture->LoadFromFile(texturePath))
	{
		NazaraError("Failed to load texture from \"" + texturePath + '"');
		return false;
	}

	texture->SetPersistent(false);

	SetAlphaMap(texture.get());
	texture.release();

	return true;
}

void NzMaterial::SetAlphaMap(NzTexture* map)
{
	m_alphaMap = map;

	InvalidateShaders(nzShaderTarget_Model);
}

void NzMaterial::SetAlphaThreshold(float alphaThreshold)
{
	m_alphaThreshold = alphaThreshold;
}

void NzMaterial::SetAmbientColor(const NzColor& ambient)
{
	m_ambientColor = ambient;
}

void NzMaterial::SetDepthFunc(nzRendererComparison depthFunc)
{
	m_states.depthFunc = depthFunc;
}

void NzMaterial::SetDiffuseColor(const NzColor& diffuse)
{
	m_diffuseColor = diffuse;
}

bool NzMaterial::SetDiffuseMap(const NzString& texturePath)
{
	std::unique_ptr<NzTexture> texture(new NzTexture);
	if (!texture->LoadFromFile(texturePath))
	{
		NazaraError("Failed to load texture from \"" + texturePath + '"');
		return false;
	}

	texture->SetPersistent(false);

	SetDiffuseMap(texture.get());
	texture.release();

	return true;
}

void NzMaterial::SetDiffuseMap(NzTexture* map)
{
	m_diffuseMap = map;

	InvalidateShaders(nzShaderTarget_Model);
}

void NzMaterial::SetDiffuseSampler(const NzTextureSampler& sampler)
{
	m_diffuseSampler = sampler;
}

void NzMaterial::SetDstBlend(nzBlendFunc func)
{
	m_states.dstBlend = func;
}

bool NzMaterial::SetEmissiveMap(const NzString& texturePath)
{
	std::unique_ptr<NzTexture> texture(new NzTexture);
	if (!texture->LoadFromFile(texturePath))
	{
		NazaraError("Failed to load texture from \"" + texturePath + '"');
		return false;
	}

	texture->SetPersistent(false);

	SetEmissiveMap(texture.get());
	texture.release();

	return true;
}

void NzMaterial::SetEmissiveMap(NzTexture* map)
{
	m_emissiveMap = map;

	InvalidateShaders(nzShaderTarget_Model);
}

void NzMaterial::SetFaceCulling(nzFaceCulling culling)
{
	m_states.faceCulling = culling;
}

void NzMaterial::SetFaceFilling(nzFaceFilling filling)
{
	m_states.faceFilling = filling;
}

bool NzMaterial::SetHeightMap(const NzString& texturePath)
{
	std::unique_ptr<NzTexture> texture(new NzTexture);
	if (!texture->LoadFromFile(texturePath))
	{
		NazaraError("Failed to load texture from \"" + texturePath + '"');
		return false;
	}

	texture->SetPersistent(false);

	SetHeightMap(texture.get());
	texture.release();

	return true;
}

void NzMaterial::SetHeightMap(NzTexture* map)
{
	m_heightMap = map;

	InvalidateShaders(nzShaderTarget_Model);
}

bool NzMaterial::SetNormalMap(const NzString& texturePath)
{
	std::unique_ptr<NzTexture> texture(new NzTexture);
	if (!texture->LoadFromFile(texturePath))
	{
		NazaraError("Failed to load texture from \"" + texturePath + '"');
		return false;
	}

	texture->SetPersistent(false);

	SetNormalMap(texture.get());
	texture.release();

	return true;
}

void NzMaterial::SetNormalMap(NzTexture* map)
{
	m_normalMap = map;

	InvalidateShaders(nzShaderTarget_Model);
}

void NzMaterial::SetRenderStates(const NzRenderStates& states)
{
	m_states = states;
}

void NzMaterial::SetShader(nzShaderTarget target, nzUInt32 flags, const NzShader* shader)
{
	ShaderUnit& unit = m_shaders[target][flags];

	unit.custom = (shader != nullptr);
	unit.shader = shader;
}

void NzMaterial::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void NzMaterial::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

bool NzMaterial::SetSpecularMap(const NzString& texturePath)
{
	std::unique_ptr<NzTexture> texture(new NzTexture);
	if (!texture->LoadFromFile(texturePath))
	{
		NazaraError("Failed to load texture from \"" + texturePath + '"');
		return false;
	}

	texture->SetPersistent(false);

	SetSpecularMap(texture.get());
	texture.release();

	return true;
}

void NzMaterial::SetSpecularMap(NzTexture* map)
{
	m_specularMap = map;

	InvalidateShaders(nzShaderTarget_Model);
}

void NzMaterial::SetSpecularSampler(const NzTextureSampler& sampler)
{
	m_specularSampler = sampler;
}

void NzMaterial::SetSrcBlend(nzBlendFunc func)
{
	m_states.srcBlend = func;
}

NzMaterial& NzMaterial::operator=(const NzMaterial& material)
{
	Copy(material);

	return *this;
}

NzMaterial& NzMaterial::operator=(NzMaterial&& material)
{
	Copy(material);

	// Comme ça nous volons la référence du matériau
	material.m_alphaMap.Reset();
	material.m_diffuseMap.Reset();
	material.m_emissiveMap.Reset();
	material.m_heightMap.Reset();
	material.m_normalMap.Reset();
	material.m_specularMap.Reset();

	for (unsigned int i = 0; i <= nzShaderTarget_Max; ++i)
		for (ShaderUnit& unit : material.m_shaders[i])
			unit.shader.Reset();

	return *this;
}

NzMaterial* NzMaterial::GetDefault()
{
	return s_defaultMaterial;
}

void NzMaterial::Copy(const NzMaterial& material)
{
	m_alphaMap.Reset();
	m_diffuseMap.Reset();
	m_emissiveMap.Reset();
	m_heightMap.Reset();
	m_normalMap.Reset();
	m_specularMap.Reset();

	for (unsigned int i = 0; i <= nzShaderTarget_Max; ++i)
		for (ShaderUnit& unit : m_shaders[i])
			unit.shader.Reset();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et bien plus rapide

	// Ensuite une petite astuce pour récupérer correctement les références
	m_alphaMap.Release();
	m_diffuseMap.Release();
	m_emissiveMap.Release();
	m_heightMap.Release();
	m_normalMap.Release();
	m_specularMap.Release();

	m_alphaMap = material.m_alphaMap;
	m_diffuseMap = material.m_diffuseMap;
	m_emissiveMap = material.m_emissiveMap;
	m_heightMap = material.m_heightMap;
	m_normalMap = material.m_normalMap;
	m_specularMap = material.m_specularMap;

	for (unsigned int i = 0; i <= nzShaderTarget_Max; ++i)
	{
		for (unsigned int j = 0; j <= nzShaderFlags_Max; ++j)
		{
			NzShaderConstRef& shader = m_shaders[i][j].shader;

			shader.Release();
			shader = material.m_shaders[i][j].shader;
		}
	}
}

void NzMaterial::GenerateShader(nzShaderTarget target, nzUInt32 flags) const
{
	NzShaderManagerParams params;
	params.target = target;
	params.flags = flags;

	switch (target)
	{
		case nzShaderTarget_FullscreenQuad:
            params.fullscreenQuad.alphaMapping = m_alphaMap.IsValid();
            params.fullscreenQuad.alphaTest = m_alphaTestEnabled;
			params.fullscreenQuad.diffuseMapping = m_diffuseMap.IsValid();
			break;

		case nzShaderTarget_Model:
			params.model.alphaMapping = m_alphaMap.IsValid();
			params.model.alphaTest = m_alphaTestEnabled;
			params.model.diffuseMapping = m_diffuseMap.IsValid();
			params.model.emissiveMapping = m_emissiveMap.IsValid();
			params.model.lighting = m_lightingEnabled;
			params.model.normalMapping = m_normalMap.IsValid();
			params.model.parallaxMapping = m_heightMap.IsValid();
			params.model.specularMapping = m_specularMap.IsValid();
			break;

		case nzShaderTarget_None:
			break;
	}

	m_shaders[target][flags].shader = NzShaderManager::Get(params);
}

void NzMaterial::InvalidateShaders(nzShaderTarget target)
{
	for (ShaderUnit& unit : m_shaders[target])
	{
		if (!unit.custom)
			unit.shader.Reset();
	}
}

bool NzMaterial::Initialize()
{
	s_defaultMaterial = new NzMaterial;

	return true;
}

void NzMaterial::Uninitialize()
{
	delete s_defaultMaterial;
	s_defaultMaterial = nullptr;
}

NzMaterial* NzMaterial::s_defaultMaterial = nullptr;
NzMaterialLoader::LoaderList NzMaterial::s_loaders;
