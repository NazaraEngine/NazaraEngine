// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Nécessaire pour inclure les headers OpenGL
#endif

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	const nzUInt8 r_coreFragmentShader[] = {
		#include <Nazara/Graphics/Resources/Shaders/Basic/core.frag.h>
	};

	const nzUInt8 r_coreVertexShader[] = {
		#include <Nazara/Graphics/Resources/Shaders/Basic/core.vert.h>
	};

	const nzUInt8 r_compatibilityFragmentShader[] = {
		#include <Nazara/Graphics/Resources/Shaders/Basic/compatibility.frag.h>
	};

	const nzUInt8 r_compatibilityVertexShader[] = {
		#include <Nazara/Graphics/Resources/Shaders/Basic/compatibility.vert.h>
	};
}

bool NzMaterialParams::IsValid() const
{
	if (!NzUberShaderLibrary::Has(shaderName))
		return false;

	return true;
}

NzMaterial::NzMaterial()
{
	Reset();
}

NzMaterial::NzMaterial(const NzMaterial& material) :
NzRefCounted(),
NzResource(material)
{
	Copy(material);
}

NzMaterial::~NzMaterial()
{
	NotifyDestroy();
}

const NzShader* NzMaterial::Apply(nzUInt32 shaderFlags, nzUInt8 textureUnit, nzUInt8* lastUsedUnit) const
{
	const ShaderInstance& instance = m_shaders[shaderFlags];
	if (!instance.uberInstance)
		GenerateShader(shaderFlags);

	instance.uberInstance->Activate();

	if (instance.uniforms[nzMaterialUniform_AlphaThreshold] != -1)
		instance.shader->SendFloat(instance.uniforms[nzMaterialUniform_AlphaThreshold], m_alphaThreshold);

	if (instance.uniforms[nzMaterialUniform_Ambient] != -1)
		instance.shader->SendColor(instance.uniforms[nzMaterialUniform_Ambient], m_ambientColor);

	if (instance.uniforms[nzMaterialUniform_Diffuse] != -1)
		instance.shader->SendColor(instance.uniforms[nzMaterialUniform_Diffuse], m_diffuseColor);

	if (instance.uniforms[nzMaterialUniform_Shininess] != -1)
		instance.shader->SendFloat(instance.uniforms[nzMaterialUniform_Shininess], m_shininess);

	if (instance.uniforms[nzMaterialUniform_Specular] != -1)
		instance.shader->SendColor(instance.uniforms[nzMaterialUniform_Specular], m_specularColor);

	if (m_alphaMap && instance.uniforms[nzMaterialUniform_AlphaMap] != -1)
	{
		NzRenderer::SetTexture(textureUnit, m_alphaMap);
		NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		instance.shader->SendInteger(instance.uniforms[nzMaterialUniform_AlphaMap], textureUnit);
		textureUnit++;
	}

	if (m_diffuseMap && instance.uniforms[nzMaterialUniform_DiffuseMap] != -1)
	{
		NzRenderer::SetTexture(textureUnit, m_diffuseMap);
		NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		instance.shader->SendInteger(instance.uniforms[nzMaterialUniform_DiffuseMap], textureUnit);
		textureUnit++;
	}

	if (m_emissiveMap && instance.uniforms[nzMaterialUniform_EmissiveMap] != -1)
	{
		NzRenderer::SetTexture(textureUnit, m_emissiveMap);
		NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		instance.shader->SendInteger(instance.uniforms[nzMaterialUniform_EmissiveMap], textureUnit);
		textureUnit++;
	}

	if (m_heightMap && instance.uniforms[nzMaterialUniform_HeightMap] != -1)
	{
		NzRenderer::SetTexture(textureUnit, m_heightMap);
		NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		instance.shader->SendInteger(instance.uniforms[nzMaterialUniform_HeightMap], textureUnit);
		textureUnit++;
	}

	if (m_normalMap && instance.uniforms[nzMaterialUniform_NormalMap] != -1)
	{
		NzRenderer::SetTexture(textureUnit, m_normalMap);
		NzRenderer::SetTextureSampler(textureUnit, m_diffuseSampler);
		instance.shader->SendInteger(instance.uniforms[nzMaterialUniform_NormalMap], textureUnit);
		textureUnit++;
	}

	if (m_specularMap && instance.uniforms[nzMaterialUniform_SpecularMap] != -1)
	{
		NzRenderer::SetTexture(textureUnit, m_specularMap);
		NzRenderer::SetTextureSampler(textureUnit, m_specularSampler);
		instance.shader->SendInteger(instance.uniforms[nzMaterialUniform_SpecularMap], textureUnit);
		textureUnit++;
	}

	NzRenderer::SetRenderStates(m_states);

	if (lastUsedUnit)
		*lastUsedUnit = textureUnit;

	return instance.shader;
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

	InvalidateShaders();
}

void NzMaterial::EnableDepthSorting(bool depthSorting)
{
	m_depthSortingEnabled = depthSorting;
}

void NzMaterial::EnableLighting(bool lighting)
{
	m_lightingEnabled = lighting;

	InvalidateShaders();
}

void NzMaterial::EnableTransform(bool transform)
{
	m_transformEnabled = transform;

	InvalidateShaders();
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

nzFaceSide NzMaterial::GetFaceCulling() const
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

const NzUberShader* NzMaterial::GetShader() const
{
	return m_uberShader;
}

const NzUberShaderInstance* NzMaterial::GetShaderInstance(nzUInt32 flags) const
{
	const ShaderInstance& instance = m_shaders[flags];
	if (!instance.uberInstance)
		GenerateShader(flags);

    return instance.uberInstance;
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

bool NzMaterial::IsDepthSortingEnabled() const
{
	return m_depthSortingEnabled;
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

bool NzMaterial::IsTransformEnabled() const
{
	return m_transformEnabled;
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
	NotifyDestroy();

	m_alphaMap.Reset();
	m_diffuseMap.Reset();
	m_emissiveMap.Reset();
	m_heightMap.Reset();
	m_normalMap.Reset();
	m_specularMap.Reset();
	m_uberShader.Reset();

	for (ShaderInstance& instance : m_shaders)
		instance.uberInstance = nullptr;

	m_alphaThreshold = 0.2f;
	m_alphaTestEnabled = false;
	m_ambientColor = NzColor(128, 128, 128);
	m_depthSortingEnabled = false;
	m_diffuseColor = NzColor::White;
	m_diffuseSampler = NzTextureSampler();
	m_lightingEnabled = true;
	m_shininess = 50.f;
	m_specularColor = NzColor::White;
	m_specularSampler = NzTextureSampler();
	m_states = NzRenderStates();
	m_states.parameters[nzRendererParameter_DepthBuffer] = true;
	m_states.parameters[nzRendererParameter_FaceCulling] = true;
	m_transformEnabled = true;

	SetShader("Basic");
}

bool NzMaterial::SetAlphaMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetAlphaMap(std::move(texture));
	return true;
}

void NzMaterial::SetAlphaMap(NzTextureRef alphaMap)
{
	m_alphaMap = std::move(alphaMap);

	InvalidateShaders();
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

bool NzMaterial::SetDiffuseMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetDiffuseMap(std::move(texture));
	return true;
}

void NzMaterial::SetDiffuseMap(NzTextureRef diffuseMap)
{
	m_diffuseMap = std::move(diffuseMap);

	InvalidateShaders();
}

void NzMaterial::SetDiffuseSampler(const NzTextureSampler& sampler)
{
	m_diffuseSampler = sampler;
}

void NzMaterial::SetDstBlend(nzBlendFunc func)
{
	m_states.dstBlend = func;
}

bool NzMaterial::SetEmissiveMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetEmissiveMap(std::move(texture));
	return true;
}

void NzMaterial::SetEmissiveMap(NzTextureRef emissiveMap)
{
	m_emissiveMap = std::move(emissiveMap);

	InvalidateShaders();
}

void NzMaterial::SetFaceCulling(nzFaceSide faceSide)
{
	m_states.faceCulling = faceSide;
}

void NzMaterial::SetFaceFilling(nzFaceFilling filling)
{
	m_states.faceFilling = filling;
}

bool NzMaterial::SetHeightMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetHeightMap(std::move(texture));
	return true;
}

void NzMaterial::SetHeightMap(NzTextureRef heightMap)
{
	m_heightMap = std::move(heightMap);

	InvalidateShaders();
}

bool NzMaterial::SetNormalMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetNormalMap(std::move(texture));
	return true;
}

void NzMaterial::SetNormalMap(NzTextureRef normalMap)
{
	m_normalMap = std::move(normalMap);

	InvalidateShaders();
}

void NzMaterial::SetRenderStates(const NzRenderStates& states)
{
	m_states = states;
}

void NzMaterial::SetShader(NzUberShaderConstRef uberShader)
{
	m_uberShader = std::move(uberShader);

	InvalidateShaders();
}

bool NzMaterial::SetShader(const NzString& uberShaderName)
{
	NzUberShaderConstRef uberShader = NzUberShaderLibrary::Get(uberShaderName);
	if (!uberShader)
		return false;

	SetShader(std::move(uberShader));
	return true;
}

void NzMaterial::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void NzMaterial::SetSpecularColor(const NzColor& specular)
{
	m_specularColor = specular;
}

bool NzMaterial::SetSpecularMap(const NzString& textureName)
{
	NzTextureRef texture = NzTextureLibrary::Query(textureName);
	if (!texture)
	{
		texture = NzTextureManager::Get(textureName);
		if (!texture)
			return false;
	}

	SetSpecularMap(std::move(texture));
	return true;
}

void NzMaterial::SetSpecularMap(NzTextureRef specularMap)
{
	m_specularMap = std::move(specularMap);

	InvalidateShaders();
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
	NzResource::operator=(material);

	Copy(material);
	return *this;
}

NzMaterialRef NzMaterial::GetDefault()
{
	return s_defaultMaterial;
}

void NzMaterial::Copy(const NzMaterial& material)
{
	// Copie des états de base
	m_alphaTestEnabled    = material.m_alphaTestEnabled;
	m_alphaThreshold      = material.m_alphaThreshold;
	m_ambientColor        = material.m_ambientColor;
	m_depthSortingEnabled = material.m_depthSortingEnabled;
	m_diffuseColor        = material.m_diffuseColor;
	m_diffuseSampler      = material.m_diffuseSampler;
	m_lightingEnabled     = material.m_lightingEnabled;
	m_shininess           = material.m_shininess;
	m_specularColor       = material.m_specularColor;
	m_specularSampler     = material.m_specularSampler;
	m_states              = material.m_states;
	m_transformEnabled    = material.m_transformEnabled;

	// Copie des références de texture
	m_alphaMap    = material.m_alphaMap;
	m_diffuseMap  = material.m_diffuseMap;
	m_emissiveMap = material.m_emissiveMap;
	m_heightMap   = material.m_heightMap;
	m_normalMap   = material.m_normalMap;
	m_specularMap = material.m_specularMap;

	// Copie de la référence vers l'Über-Shader
	m_uberShader = material.m_uberShader;

	// On copie les instances de shader par la même occasion
	std::memcpy(&m_shaders[0], &material.m_shaders[0], (nzShaderFlags_Max+1)*sizeof(ShaderInstance));
}

void NzMaterial::GenerateShader(nzUInt32 flags) const
{
	NzParameterList list;
	list.SetParameter("ALPHA_MAPPING", m_alphaMap.IsValid());
	list.SetParameter("ALPHA_TEST", m_alphaTestEnabled);
	list.SetParameter("COMPUTE_TBNMATRIX", m_normalMap.IsValid() || m_heightMap.IsValid());
	list.SetParameter("DIFFUSE_MAPPING", m_diffuseMap.IsValid());
	list.SetParameter("EMISSIVE_MAPPING", m_emissiveMap.IsValid());
	list.SetParameter("LIGHTING", m_lightingEnabled);
	list.SetParameter("NORMAL_MAPPING", m_normalMap.IsValid());
	list.SetParameter("PARALLAX_MAPPING", m_heightMap.IsValid());
	list.SetParameter("SPECULAR_MAPPING", m_specularMap.IsValid());
	list.SetParameter("TEXTURE_MAPPING", m_alphaMap.IsValid() || m_diffuseMap.IsValid() || m_emissiveMap.IsValid() ||
	                                     m_normalMap.IsValid() || m_heightMap.IsValid() || m_specularMap.IsValid() ||
	                                     flags & nzShaderFlags_TextureOverlay);
	list.SetParameter("TRANSFORM", m_transformEnabled);

	list.SetParameter("FLAG_BILLBOARD", static_cast<bool>(flags & nzShaderFlags_Billboard));
	list.SetParameter("FLAG_DEFERRED", static_cast<bool>((flags & nzShaderFlags_Deferred) != 0));
	list.SetParameter("FLAG_INSTANCING", static_cast<bool>((flags & nzShaderFlags_Instancing) != 0));
	list.SetParameter("FLAG_TEXTUREOVERLAY", static_cast<bool>((flags & nzShaderFlags_TextureOverlay) != 0));
	list.SetParameter("FLAG_VERTEXCOLOR", static_cast<bool>((flags & nzShaderFlags_VertexColor) != 0));

	ShaderInstance& instance = m_shaders[flags];
	instance.uberInstance = m_uberShader->Get(list);
	instance.shader = instance.uberInstance->GetShader();

	#define CacheUniform(name) instance.uniforms[nzMaterialUniform_##name] = instance.shader->GetUniformLocation("Material" #name)

	CacheUniform(AlphaMap);
	CacheUniform(AlphaThreshold);
	CacheUniform(Ambient);
	CacheUniform(Diffuse);
	CacheUniform(DiffuseMap);
	CacheUniform(EmissiveMap);
	CacheUniform(HeightMap);
	CacheUniform(NormalMap);
	CacheUniform(Shininess);
	CacheUniform(Specular);
	CacheUniform(SpecularMap);

	#undef CacheUniform
}

void NzMaterial::InvalidateShaders()
{
	for (ShaderInstance& instance : m_shaders)
		instance.uberInstance = nullptr;
}

bool NzMaterial::Initialize()
{
	if (!NzMaterialLibrary::Initialize())
	{
		NazaraError("Failed to initialise library");
		return false;
	}

	if (!NzMaterialManager::Initialize())
	{
		NazaraError("Failed to initialise manager");
		return false;
	}

	bool glsl140 = (NzOpenGL::GetGLSLVersion() >= 140);

	// Basic shader
	{
		NzUberShaderPreprocessorRef uberShader = NzUberShaderPreprocessor::New();

		NzString fragmentShader;
		NzString vertexShader;
		if (glsl140)
		{
			fragmentShader.Set(reinterpret_cast<const char*>(r_coreFragmentShader), sizeof(r_coreFragmentShader));
			vertexShader.Set(reinterpret_cast<const char*>(r_coreVertexShader), sizeof(r_coreVertexShader));
		}
		else
		{
			fragmentShader.Set(reinterpret_cast<const char*>(r_compatibilityFragmentShader), sizeof(r_compatibilityFragmentShader));
			vertexShader.Set(reinterpret_cast<const char*>(r_compatibilityVertexShader), sizeof(r_compatibilityVertexShader));
		}

		uberShader->SetShader(nzShaderStage_Fragment, fragmentShader, "FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING");
		uberShader->SetShader(nzShaderStage_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_INSTANCING FLAG_VERTEXCOLOR TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

		NzUberShaderLibrary::Register("Basic", uberShader);
	}

	// PhongLighting shader
	{
		NzUberShaderPreprocessorRef uberShader = NzUberShaderPreprocessor::New();

		NzString fragmentShader;
		NzString vertexShader;
		if (glsl140)
		{
			const nzUInt8 coreFragmentShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/PhongLighting/core.frag.h>
			};

			const nzUInt8 coreVertexShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/PhongLighting/core.vert.h>
			};

			fragmentShader.Set(reinterpret_cast<const char*>(coreFragmentShader), sizeof(coreFragmentShader));
			vertexShader.Set(reinterpret_cast<const char*>(coreVertexShader), sizeof(coreVertexShader));
		}
		else
		{
			const nzUInt8 compatibilityFragmentShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/PhongLighting/compatibility.frag.h>
			};

			const nzUInt8 compatibilityVertexShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/PhongLighting/compatibility.vert.h>
			};

			fragmentShader.Set(reinterpret_cast<const char*>(compatibilityFragmentShader), sizeof(compatibilityFragmentShader));
			vertexShader.Set(reinterpret_cast<const char*>(compatibilityVertexShader), sizeof(compatibilityVertexShader));
		}

		uberShader->SetShader(nzShaderStage_Fragment, fragmentShader, "FLAG_DEFERRED FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING LIGHTING NORMAL_MAPPING PARALLAX_MAPPING SPECULAR_MAPPING");
		uberShader->SetShader(nzShaderStage_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX LIGHTING PARALLAX_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

		NzUberShaderLibrary::Register("PhongLighting", uberShader);
	}

	// Une fois les shaders de base enregistrés, on peut créer le matériau par défaut
	s_defaultMaterial = NzMaterial::New();
	s_defaultMaterial->Enable(nzRendererParameter_FaceCulling, false);
	s_defaultMaterial->SetFaceFilling(nzFaceFilling_Line);
	NzMaterialLibrary::Register("Default", s_defaultMaterial);

	return true;
}

void NzMaterial::Uninitialize()
{
	s_defaultMaterial.Reset();
	NzUberShaderLibrary::Unregister("PhongLighting");
	NzUberShaderLibrary::Unregister("Basic");
	NzMaterialManager::Uninitialize();
	NzMaterialLibrary::Uninitialize();
}

NzMaterialLibrary::LibraryMap NzMaterial::s_library;
NzMaterialLoader::LoaderList NzMaterial::s_loaders;
NzMaterialManager::ManagerMap NzMaterial::s_managerMap;
NzMaterialManager::ManagerParams NzMaterial::s_managerParameters;
NzMaterialRef NzMaterial::s_defaultMaterial = nullptr;
