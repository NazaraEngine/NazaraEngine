// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Nécessaire pour inclure les headers OpenGL
#endif

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UberShaderLibrary.hpp>
#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

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
	material.m_uberShader.Reset();
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

	InvalidateShaders();
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

	InvalidateShaders();
}

void NzMaterial::SetRenderStates(const NzRenderStates& states)
{
	m_states = states;
}

void NzMaterial::SetShader(const NzUberShader* uberShader)
{
	m_uberShader = uberShader;

	InvalidateShaders();
}

bool NzMaterial::SetShader(const NzString& uberShaderName)
{
	NzUberShader* uberShader = NzUberShaderLibrary::Get(uberShaderName);
	if (!uberShader)
		return false;

	SetShader(uberShader);
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
	material.m_uberShader.Reset();

	return *this;
}

NzMaterial* NzMaterial::GetDefault()
{
	return s_defaultMaterial;
}

void NzMaterial::Copy(const NzMaterial& material)
{
	// On relache les références proprement
	m_alphaMap.Reset();
	m_diffuseMap.Reset();
	m_emissiveMap.Reset();
	m_heightMap.Reset();
	m_normalMap.Reset();
	m_specularMap.Reset();
	m_uberShader.Reset();

	std::memcpy(this, &material, sizeof(NzMaterial)); // Autorisé dans notre cas, et bien plus rapide

	// Ensuite une petite astuce pour récupérer correctement les références
	m_alphaMap.Release();
	m_diffuseMap.Release();
	m_emissiveMap.Release();
	m_heightMap.Release();
	m_normalMap.Release();
	m_specularMap.Release();
	m_uberShader.Release();

	m_alphaMap = material.m_alphaMap;
	m_diffuseMap = material.m_diffuseMap;
	m_emissiveMap = material.m_emissiveMap;
	m_heightMap = material.m_heightMap;
	m_normalMap = material.m_normalMap;
	m_specularMap = material.m_specularMap;
	m_uberShader = material.m_uberShader;
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
	                                     m_normalMap.IsValid() || m_heightMap.IsValid() || m_specularMap.IsValid());
	list.SetParameter("TRANSFORM", m_transformEnabled);

	list.SetParameter("FLAG_BILLBOARD", static_cast<bool>(flags & nzShaderFlags_Billboard));
	list.SetParameter("FLAG_DEFERRED", static_cast<bool>(flags & nzShaderFlags_Deferred));
	list.SetParameter("FLAG_INSTANCING", static_cast<bool>(flags & nzShaderFlags_Instancing));
	list.SetParameter("FLAG_VERTEXCOLOR", static_cast<bool>(flags & nzShaderFlags_VertexColor));

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
	bool glsl140 = (NzOpenGL::GetGLSLVersion() >= 140);

	// Basic shader
	{
		std::unique_ptr<NzUberShaderPreprocessor> uberShader(new NzUberShaderPreprocessor);
		uberShader->SetPersistent(false);

		NzString fragmentShader;
		NzString vertexShader;
		if (glsl140)
		{
			const nzUInt8 coreFragmentShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/Basic/core.frag.h>
			};

			const nzUInt8 coreVertexShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/Basic/core.vert.h>
			};

			fragmentShader.Set(reinterpret_cast<const char*>(coreFragmentShader), sizeof(coreFragmentShader));
			vertexShader.Set(reinterpret_cast<const char*>(coreVertexShader), sizeof(coreVertexShader));
		}
		else
		{
			const nzUInt8 compatibilityFragmentShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/Basic/compatibility.frag.h>
			};

			const nzUInt8 compatibilityVertexShader[] = {
				#include <Nazara/Graphics/Resources/Shaders/Basic/compatibility.vert.h>
			};

			fragmentShader.Set(reinterpret_cast<const char*>(compatibilityFragmentShader), sizeof(compatibilityFragmentShader));
			vertexShader.Set(reinterpret_cast<const char*>(compatibilityVertexShader), sizeof(compatibilityVertexShader));
		}

		uberShader->SetShader(nzShaderStage_Fragment, fragmentShader, "ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING");
		uberShader->SetShader(nzShaderStage_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_INSTANCING FLAG_VERTEXCOLOR TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

		NzUberShaderLibrary::Register("Basic", uberShader.get());
		uberShader.release();
	}

	// PhongLighting shader
	{
		std::unique_ptr<NzUberShaderPreprocessor> uberShader(new NzUberShaderPreprocessor);
		uberShader->SetPersistent(false);

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

		uberShader->SetShader(nzShaderStage_Fragment, fragmentShader, "FLAG_DEFERRED ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING LIGHTING NORMAL_MAPPING PARALLAX_MAPPING SPECULAR_MAPPING");
		uberShader->SetShader(nzShaderStage_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX LIGHTING PARALLAX_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

		NzUberShaderLibrary::Register("PhongLighting", uberShader.get());
		uberShader.release();
	}

	s_defaultMaterial = new NzMaterial;
	s_defaultMaterial->SetPersistent(true);

	s_defaultMaterial->Enable(nzRendererParameter_FaceCulling, false);
	s_defaultMaterial->SetFaceFilling(nzFaceFilling_Line);

	return true;
}

void NzMaterial::Uninitialize()
{
	NzUberShaderLibrary::Unregister("PhongLighting");
	NzUberShaderLibrary::Unregister("Basic");

	s_defaultMaterial->SetPersistent(false, true);
	s_defaultMaterial = nullptr;
}

NzMaterial* NzMaterial::s_defaultMaterial = nullptr;
NzMaterialLoader::LoaderList NzMaterial::s_loaders;
