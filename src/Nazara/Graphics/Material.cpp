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

void NzMaterial::Reset()
{
	OnMaterialReset(this);

	m_alphaMap.Reset();
	m_depthMaterial.Reset();
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
	m_shadowCastingEnabled = true;
	m_shadowReceiveEnabled = true;
	m_shininess = 50.f;
	m_specularColor = NzColor::White;
	m_specularSampler = NzTextureSampler();
	m_states = NzRenderStates();
	m_states.parameters[nzRendererParameter_DepthBuffer] = true;
	m_states.parameters[nzRendererParameter_FaceCulling] = true;
	m_transformEnabled = true;

	SetShader("Basic");
}

void NzMaterial::Copy(const NzMaterial& material)
{
	// Copie des états de base
	m_alphaTestEnabled     = material.m_alphaTestEnabled;
	m_alphaThreshold       = material.m_alphaThreshold;
	m_ambientColor         = material.m_ambientColor;
	m_depthSortingEnabled  = material.m_depthSortingEnabled;
	m_diffuseColor         = material.m_diffuseColor;
	m_diffuseSampler       = material.m_diffuseSampler;
	m_lightingEnabled      = material.m_lightingEnabled;
	m_shininess            = material.m_shininess;
	m_shadowCastingEnabled = material.m_shadowCastingEnabled;
	m_shadowReceiveEnabled = material.m_shadowReceiveEnabled;
	m_specularColor        = material.m_specularColor;
	m_specularSampler      = material.m_specularSampler;
	m_states               = material.m_states;
	m_transformEnabled     = material.m_transformEnabled;

	// Copying resources refs
	m_alphaMap      = material.m_alphaMap;
	m_depthMaterial = material.m_depthMaterial;
	m_diffuseMap    = material.m_diffuseMap;
	m_emissiveMap   = material.m_emissiveMap;
	m_heightMap     = material.m_heightMap;
	m_normalMap     = material.m_normalMap;
	m_specularMap   = material.m_specularMap;
	m_uberShader    = material.m_uberShader;

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
	list.SetParameter("SHADOW_MAPPING", m_shadowReceiveEnabled);
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

		uberShader->SetShader(nzShaderStage_Fragment, fragmentShader, "FLAG_DEFERRED FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING LIGHTING NORMAL_MAPPING PARALLAX_MAPPING SHADOW_MAPPING SPECULAR_MAPPING");
		uberShader->SetShader(nzShaderStage_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX LIGHTING PARALLAX_MAPPING SHADOW_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

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
