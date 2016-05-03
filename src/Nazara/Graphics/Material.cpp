// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Nécessaire pour inclure les headers OpenGL
#endif

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
#include <cstring>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	namespace
	{
		const UInt8 r_basicFragmentShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/Basic/core.frag.h>
		};

		const UInt8 r_basicVertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/Basic/core.vert.h>
		};

		const UInt8 r_phongLightingFragmentShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhongLighting/core.frag.h>
		};

		const UInt8 r_phongLightingVertexShader[] = {
			#include <Nazara/Graphics/Resources/Shaders/PhongLighting/core.vert.h>
		};
	}

	bool MaterialParams::IsValid() const
	{
		if (!UberShaderLibrary::Has(shaderName))
			return false;

		return true;
	}

	Material::Material()
	{
		Reset();
	}

	Material::Material(const Material& material) :
	RefCounted(),
	Resource(material)
	{
		Copy(material);
	}

	Material::~Material()
	{
		OnMaterialRelease(this);
	}

	const Shader* Material::Apply(UInt32 shaderFlags, UInt8 textureUnit, UInt8* lastUsedUnit) const
	{
		const ShaderInstance& instance = m_shaders[shaderFlags];
		if (!instance.uberInstance)
			GenerateShader(shaderFlags);

		instance.uberInstance->Activate();

		if (instance.uniforms[MaterialUniform_AlphaThreshold] != -1)
			instance.shader->SendFloat(instance.uniforms[MaterialUniform_AlphaThreshold], m_alphaThreshold);

		if (instance.uniforms[MaterialUniform_Ambient] != -1)
			instance.shader->SendColor(instance.uniforms[MaterialUniform_Ambient], m_ambientColor);

		if (instance.uniforms[MaterialUniform_Diffuse] != -1)
			instance.shader->SendColor(instance.uniforms[MaterialUniform_Diffuse], m_diffuseColor);

		if (instance.uniforms[MaterialUniform_Shininess] != -1)
			instance.shader->SendFloat(instance.uniforms[MaterialUniform_Shininess], m_shininess);

		if (instance.uniforms[MaterialUniform_Specular] != -1)
			instance.shader->SendColor(instance.uniforms[MaterialUniform_Specular], m_specularColor);

		if (m_alphaMap && instance.uniforms[MaterialUniform_AlphaMap] != -1)
		{
			Renderer::SetTexture(textureUnit, m_alphaMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			instance.shader->SendInteger(instance.uniforms[MaterialUniform_AlphaMap], textureUnit);
			textureUnit++;
		}

		if (m_diffuseMap && instance.uniforms[MaterialUniform_DiffuseMap] != -1)
		{
			Renderer::SetTexture(textureUnit, m_diffuseMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			instance.shader->SendInteger(instance.uniforms[MaterialUniform_DiffuseMap], textureUnit);
			textureUnit++;
		}

		if (m_emissiveMap && instance.uniforms[MaterialUniform_EmissiveMap] != -1)
		{
			Renderer::SetTexture(textureUnit, m_emissiveMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			instance.shader->SendInteger(instance.uniforms[MaterialUniform_EmissiveMap], textureUnit);
			textureUnit++;
		}

		if (m_heightMap && instance.uniforms[MaterialUniform_HeightMap] != -1)
		{
			Renderer::SetTexture(textureUnit, m_heightMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			instance.shader->SendInteger(instance.uniforms[MaterialUniform_HeightMap], textureUnit);
			textureUnit++;
		}

		if (m_normalMap && instance.uniforms[MaterialUniform_NormalMap] != -1)
		{
			Renderer::SetTexture(textureUnit, m_normalMap);
			Renderer::SetTextureSampler(textureUnit, m_diffuseSampler);
			instance.shader->SendInteger(instance.uniforms[MaterialUniform_NormalMap], textureUnit);
			textureUnit++;
		}

		if (m_specularMap && instance.uniforms[MaterialUniform_SpecularMap] != -1)
		{
			Renderer::SetTexture(textureUnit, m_specularMap);
			Renderer::SetTextureSampler(textureUnit, m_specularSampler);
			instance.shader->SendInteger(instance.uniforms[MaterialUniform_SpecularMap], textureUnit);
			textureUnit++;
		}

		Renderer::SetRenderStates(m_states);

		if (lastUsedUnit)
			*lastUsedUnit = textureUnit;

		return instance.shader;
	}

	void Material::BuildFromParameters(const ParameterList& matData, const MaterialParams& matParams)
	{
		Color color;
		bool isEnabled;
		float fValue;
		int iValue;
		String path;

		ErrorFlags errFlags(ErrorFlag_Silent | ErrorFlag_ThrowExceptionDisabled, true);


		if (matData.GetFloatParameter(MaterialData::AlphaThreshold, &fValue))
			SetAlphaThreshold(fValue);

		if (matData.GetBooleanParameter(MaterialData::AlphaTest, &isEnabled))
			EnableAlphaTest(isEnabled);

		if (matData.GetColorParameter(MaterialData::AmbientColor, &color))
			SetAmbientColor(color);

		if (matData.GetIntegerParameter(MaterialData::DepthFunc, &iValue))
			SetDepthFunc(static_cast<RendererComparison>(iValue));

		if (matData.GetBooleanParameter(MaterialData::DepthSorting, &isEnabled))
			EnableDepthSorting(isEnabled);

		if (matData.GetColorParameter(MaterialData::DiffuseColor, &color))
			SetDiffuseColor(color);

		if (matData.GetIntegerParameter(MaterialData::DstBlend, &iValue))
			SetDstBlend(static_cast<BlendFunc>(iValue));

		if (matData.GetIntegerParameter(MaterialData::FaceCulling, &iValue))
			SetFaceCulling(static_cast<FaceSide>(iValue));

		if (matData.GetIntegerParameter(MaterialData::FaceFilling, &iValue))
			SetFaceFilling(static_cast<FaceFilling>(iValue));

		if (matData.GetBooleanParameter(MaterialData::Lighting, &isEnabled))
			EnableLighting(isEnabled);

		if (matData.GetFloatParameter(MaterialData::LineWidth, &fValue))
			m_states.lineWidth = fValue;

		if (matData.GetFloatParameter(MaterialData::PointSize, &fValue))
			m_states.pointSize = fValue;

		if (matData.GetColorParameter(MaterialData::SpecularColor, &color))
			SetSpecularColor(color);

		if (matData.GetFloatParameter(MaterialData::Shininess, &fValue))
			SetShininess(fValue);

		if (matData.GetIntegerParameter(MaterialData::SrcBlend, &iValue))
			SetSrcBlend(static_cast<BlendFunc>(iValue));

		if (matData.GetBooleanParameter(MaterialData::Transform, &isEnabled))
			EnableTransform(isEnabled);

		// RendererParameter
		if (matData.GetBooleanParameter(MaterialData::Blending, &isEnabled))
			Enable(RendererParameter_Blend, isEnabled);

		if (matData.GetBooleanParameter(MaterialData::ColorWrite, &isEnabled))
			Enable(RendererParameter_ColorWrite, isEnabled);

		if (matData.GetBooleanParameter(MaterialData::DepthBuffer, &isEnabled))
			Enable(RendererParameter_DepthBuffer, isEnabled);

		if (matData.GetBooleanParameter(MaterialData::DepthWrite, &isEnabled))
			Enable(RendererParameter_DepthWrite, isEnabled);

		if (matData.GetBooleanParameter(MaterialData::FaceCulling, &isEnabled))
			Enable(RendererParameter_FaceCulling, isEnabled);

		if (matData.GetBooleanParameter(MaterialData::ScissorTest, &isEnabled))
			Enable(RendererParameter_ScissorTest, isEnabled);

		if (matData.GetBooleanParameter(MaterialData::StencilTest, &isEnabled))
			Enable(RendererParameter_StencilTest, isEnabled);

		// Samplers
		if (matData.GetIntegerParameter(MaterialData::DiffuseAnisotropyLevel, &iValue))
			m_diffuseSampler.SetAnisotropyLevel(static_cast<UInt8>(iValue));

		if (matData.GetIntegerParameter(MaterialData::DiffuseFilter, &iValue))
			m_diffuseSampler.SetFilterMode(static_cast<SamplerFilter>(iValue));

		if (matData.GetIntegerParameter(MaterialData::DiffuseWrap, &iValue))
			m_diffuseSampler.SetWrapMode(static_cast<SamplerWrap>(iValue));

		if (matData.GetIntegerParameter(MaterialData::SpecularAnisotropyLevel, &iValue))
			m_specularSampler.SetAnisotropyLevel(static_cast<UInt8>(iValue));

		if (matData.GetIntegerParameter(MaterialData::SpecularFilter, &iValue))
			m_specularSampler.SetFilterMode(static_cast<SamplerFilter>(iValue));

		if (matData.GetIntegerParameter(MaterialData::SpecularWrap, &iValue))
			m_specularSampler.SetWrapMode(static_cast<SamplerWrap>(iValue));

		// Stencil
		if (matData.GetIntegerParameter(MaterialData::StencilCompare, &iValue))
			m_states.frontFace.stencilCompare = static_cast<RendererComparison>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilFail, &iValue))
			m_states.frontFace.stencilFail = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilPass, &iValue))
			m_states.frontFace.stencilPass = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilZFail, &iValue))
			m_states.frontFace.stencilZFail = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilMask, &iValue))
			m_states.frontFace.stencilMask = static_cast<UInt32>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilReference, &iValue))
			m_states.frontFace.stencilReference = static_cast<unsigned int>(iValue);

		// Stencil (back)
		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilCompare, &iValue))
			m_states.backFace.stencilCompare = static_cast<RendererComparison>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilFail, &iValue))
			m_states.backFace.stencilFail = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilPass, &iValue))
			m_states.backFace.stencilPass = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilZFail, &iValue))
			m_states.backFace.stencilZFail = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilMask, &iValue))
			m_states.backFace.stencilMask = static_cast<UInt32>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilReference, &iValue))
			m_states.backFace.stencilReference = static_cast<unsigned int>(iValue);

		// Textures
		if (matParams.loadAlphaMap && matData.GetStringParameter(MaterialData::AlphaTexturePath, &path))
			SetAlphaMap(path);

		if (matParams.loadDiffuseMap && matData.GetStringParameter(MaterialData::DiffuseTexturePath, &path))
			SetDiffuseMap(path);

		if (matParams.loadEmissiveMap && matData.GetStringParameter(MaterialData::EmissiveTexturePath, &path))
			SetEmissiveMap(path);

		if (matParams.loadHeightMap && matData.GetStringParameter(MaterialData::HeightTexturePath, &path))
			SetHeightMap(path);

		if (matParams.loadNormalMap && matData.GetStringParameter(MaterialData::NormalTexturePath, &path))
			SetNormalMap(path);

		if (matParams.loadSpecularMap && matData.GetStringParameter(MaterialData::SpecularTexturePath, &path))
			SetSpecularMap(path);

		SetShader(matParams.shaderName);
	}

	void Material::Enable(RendererParameter renderParameter, bool enable)
	{
		#ifdef NAZARA_DEBUG
		if (renderParameter > RendererParameter_Max)
		{
			NazaraError("Renderer parameter out of enum");
			return;
		}
		#endif

		m_states.parameters[renderParameter] = enable;
	}

	void Material::EnableAlphaTest(bool alphaTest)
	{
		m_alphaTestEnabled = alphaTest;

		InvalidateShaders();
	}

	void Material::EnableDepthSorting(bool depthSorting)
	{
		m_depthSortingEnabled = depthSorting;
	}

	void Material::EnableLighting(bool lighting)
	{
		m_lightingEnabled = lighting;

		InvalidateShaders();
	}

	void Material::EnableTransform(bool transform)
	{
		m_transformEnabled = transform;

		InvalidateShaders();
	}

	Texture* Material::GetAlphaMap() const
	{
		return m_alphaMap;
	}

	float Material::GetAlphaThreshold() const
	{
		return m_alphaThreshold;
	}

	Color Material::GetAmbientColor() const
	{
		return m_ambientColor;
	}

	RendererComparison Material::GetDepthFunc() const
	{
		return m_states.depthFunc;
	}

	Color Material::GetDiffuseColor() const
	{
		return m_diffuseColor;
	}

	TextureSampler& Material::GetDiffuseSampler()
	{
		return m_diffuseSampler;
	}

	const TextureSampler& Material::GetDiffuseSampler() const
	{
		return m_diffuseSampler;
	}

	Texture* Material::GetDiffuseMap() const
	{
		return m_diffuseMap;
	}

	BlendFunc Material::GetDstBlend() const
	{
		return m_states.dstBlend;
	}

	Texture* Material::GetEmissiveMap() const
	{
		return m_emissiveMap;
	}

	FaceSide Material::GetFaceCulling() const
	{
		return m_states.faceCulling;
	}

	FaceFilling Material::GetFaceFilling() const
	{
		return m_states.faceFilling;
	}

	Texture* Material::GetHeightMap() const
	{
		return m_heightMap;
	}

	Texture* Material::GetNormalMap() const
	{
		return m_normalMap;
	}

	const RenderStates& Material::GetRenderStates() const
	{
		return m_states;
	}

	const UberShader* Material::GetShader() const
	{
		return m_uberShader;
	}

	const UberShaderInstance* Material::GetShaderInstance(UInt32 flags) const
	{
		const ShaderInstance& instance = m_shaders[flags];
		if (!instance.uberInstance)
			GenerateShader(flags);

		return instance.uberInstance;
	}

	float Material::GetShininess() const
	{
		return m_shininess;
	}

	Color Material::GetSpecularColor() const
	{
		return m_specularColor;
	}

	Texture* Material::GetSpecularMap() const
	{
		return m_specularMap;
	}

	TextureSampler& Material::GetSpecularSampler()
	{
		return m_specularSampler;
	}

	const TextureSampler& Material::GetSpecularSampler() const
	{
		return m_specularSampler;
	}

	BlendFunc Material::GetSrcBlend() const
	{
		return m_states.srcBlend;
	}

	bool Material::HasAlphaMap() const
	{
		return m_alphaMap.IsValid();
	}

	bool Material::HasDiffuseMap() const
	{
		return m_diffuseMap.IsValid();
	}

	bool Material::HasEmissiveMap() const
	{
		return m_emissiveMap.IsValid();
	}

	bool Material::HasHeightMap() const
	{
		return m_heightMap.IsValid();
	}

	bool Material::HasNormalMap() const
	{
		return m_normalMap.IsValid();
	}

	bool Material::HasSpecularMap() const
	{
		return m_specularMap.IsValid();
	}

	bool Material::IsAlphaTestEnabled() const
	{
		return m_alphaTestEnabled;
	}

	bool Material::IsDepthSortingEnabled() const
	{
		return m_depthSortingEnabled;
	}

	bool Material::IsEnabled(RendererParameter parameter) const
	{
		#ifdef NAZARA_DEBUG
		if (parameter > RendererParameter_Max)
		{
			NazaraError("Renderer parameter out of enum");
			return false;
		}
		#endif

		return m_states.parameters[parameter];
	}

	bool Material::IsLightingEnabled() const
	{
		return m_lightingEnabled;
	}

	bool Material::IsTransformEnabled() const
	{
		return m_transformEnabled;
	}

	bool Material::LoadFromFile(const String& filePath, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromFile(this, filePath, params);
	}

	bool Material::LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromMemory(this, data, size, params);
	}

	bool Material::LoadFromStream(Stream& stream, const MaterialParams& params)
	{
		return MaterialLoader::LoadFromStream(this, stream, params);
	}

	void Material::Reset()
	{
		OnMaterialReset(this);

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
		m_ambientColor = Color(128, 128, 128);
		m_depthSortingEnabled = false;
		m_diffuseColor = Color::White;
		m_diffuseSampler = TextureSampler();
		m_lightingEnabled = true;
		m_shininess = 50.f;
		m_specularColor = Color::White;
		m_specularSampler = TextureSampler();
		m_states = RenderStates();
		m_states.parameters[RendererParameter_DepthBuffer] = true;
		m_states.parameters[RendererParameter_FaceCulling] = true;
		m_transformEnabled = true;

		SetShader("Basic");
	}

	bool Material::SetAlphaMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
				return false;
		}

		SetAlphaMap(std::move(texture));
		return true;
	}

	void Material::SetAlphaMap(TextureRef alphaMap)
	{
		m_alphaMap = std::move(alphaMap);

		InvalidateShaders();
	}

	void Material::SetAlphaThreshold(float alphaThreshold)
	{
		m_alphaThreshold = alphaThreshold;
	}

	void Material::SetAmbientColor(const Color& ambient)
	{
		m_ambientColor = ambient;
	}

	void Material::SetDepthFunc(RendererComparison depthFunc)
	{
		m_states.depthFunc = depthFunc;
	}

	void Material::SetDiffuseColor(const Color& diffuse)
	{
		m_diffuseColor = diffuse;
	}

	bool Material::SetDiffuseMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
				return false;
		}

		SetDiffuseMap(std::move(texture));
		return true;
	}

	void Material::SetDiffuseMap(TextureRef diffuseMap)
	{
		m_diffuseMap = std::move(diffuseMap);

		InvalidateShaders();
	}

	void Material::SetDiffuseSampler(const TextureSampler& sampler)
	{
		m_diffuseSampler = sampler;
	}

	void Material::SetDstBlend(BlendFunc func)
	{
		m_states.dstBlend = func;
	}

	bool Material::SetEmissiveMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
				return false;
		}

		SetEmissiveMap(std::move(texture));
		return true;
	}

	void Material::SetEmissiveMap(TextureRef emissiveMap)
	{
		m_emissiveMap = std::move(emissiveMap);

		InvalidateShaders();
	}

	void Material::SetFaceCulling(FaceSide faceSide)
	{
		m_states.faceCulling = faceSide;
	}

	void Material::SetFaceFilling(FaceFilling filling)
	{
		m_states.faceFilling = filling;
	}

	bool Material::SetHeightMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
				return false;
		}

		SetHeightMap(std::move(texture));
		return true;
	}

	void Material::SetHeightMap(TextureRef heightMap)
	{
		m_heightMap = std::move(heightMap);

		InvalidateShaders();
	}

	bool Material::SetNormalMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
				return false;
		}

		SetNormalMap(std::move(texture));
		return true;
	}

	void Material::SetNormalMap(TextureRef normalMap)
	{
		m_normalMap = std::move(normalMap);

		InvalidateShaders();
	}

	void Material::SetRenderStates(const RenderStates& states)
	{
		m_states = states;
	}

	void Material::SetShader(UberShaderConstRef uberShader)
	{
		m_uberShader = std::move(uberShader);

		InvalidateShaders();
	}

	bool Material::SetShader(const String& uberShaderName)
	{
		UberShaderConstRef uberShader = UberShaderLibrary::Get(uberShaderName);
		if (!uberShader)
			return false;

		SetShader(std::move(uberShader));
		return true;
	}

	void Material::SetShininess(float shininess)
	{
		m_shininess = shininess;
	}

	void Material::SetSpecularColor(const Color& specular)
	{
		m_specularColor = specular;
	}

	bool Material::SetSpecularMap(const String& textureName)
	{
		TextureRef texture = TextureLibrary::Query(textureName);
		if (!texture)
		{
			texture = TextureManager::Get(textureName);
			if (!texture)
				return false;
		}

		SetSpecularMap(std::move(texture));
		return true;
	}

	void Material::SetSpecularMap(TextureRef specularMap)
	{
		m_specularMap = std::move(specularMap);

		InvalidateShaders();
	}

	void Material::SetSpecularSampler(const TextureSampler& sampler)
	{
		m_specularSampler = sampler;
	}

	void Material::SetSrcBlend(BlendFunc func)
	{
		m_states.srcBlend = func;
	}

	Material& Material::operator=(const Material& material)
	{
		Resource::operator=(material);

		Copy(material);
		return *this;
	}

	MaterialRef Material::GetDefault()
	{
		return s_defaultMaterial;
	}

	void Material::Copy(const Material& material)
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
		std::memcpy(&m_shaders[0], &material.m_shaders[0], (ShaderFlags_Max+1)*sizeof(ShaderInstance));
	}

	void Material::GenerateShader(UInt32 flags) const
	{
		ParameterList list;
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
											 flags & ShaderFlags_TextureOverlay);
		list.SetParameter("TRANSFORM", m_transformEnabled);

		list.SetParameter("FLAG_BILLBOARD", static_cast<bool>((flags & ShaderFlags_Billboard) != 0));
		list.SetParameter("FLAG_DEFERRED", static_cast<bool>((flags & ShaderFlags_Deferred) != 0));
		list.SetParameter("FLAG_INSTANCING", static_cast<bool>((flags & ShaderFlags_Instancing) != 0));
		list.SetParameter("FLAG_TEXTUREOVERLAY", static_cast<bool>((flags & ShaderFlags_TextureOverlay) != 0));
		list.SetParameter("FLAG_VERTEXCOLOR", static_cast<bool>((flags & ShaderFlags_VertexColor) != 0));

		ShaderInstance& instance = m_shaders[flags];
		instance.uberInstance = m_uberShader->Get(list);
		instance.shader = instance.uberInstance->GetShader();

		#define CacheUniform(name) instance.uniforms[MaterialUniform_##name] = instance.shader->GetUniformLocation("Material" #name)

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

	void Material::InvalidateShaders()
	{
		for (ShaderInstance& instance : m_shaders)
			instance.uberInstance = nullptr;
	}

	bool Material::Initialize()
	{
		if (!MaterialLibrary::Initialize())
		{
			NazaraError("Failed to initialise library");
			return false;
		}

		if (!MaterialManager::Initialize())
		{
			NazaraError("Failed to initialise manager");
			return false;
		}

		// Basic shader
		{
			UberShaderPreprocessorRef uberShader = UberShaderPreprocessor::New();

			String fragmentShader(reinterpret_cast<const char*>(r_basicFragmentShader), sizeof(r_basicFragmentShader));
			String vertexShader(reinterpret_cast<const char*>(r_basicVertexShader), sizeof(r_basicVertexShader));

			uberShader->SetShader(ShaderStageType_Fragment, fragmentShader, "FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING");
			uberShader->SetShader(ShaderStageType_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_INSTANCING FLAG_VERTEXCOLOR TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

			UberShaderLibrary::Register("Basic", uberShader);
		}

		// PhongLighting shader
		{
			UberShaderPreprocessorRef uberShader = UberShaderPreprocessor::New();

			String fragmentShader(reinterpret_cast<const char*>(r_phongLightingFragmentShader), sizeof(r_phongLightingFragmentShader));
			String vertexShader(reinterpret_cast<const char*>(r_phongLightingVertexShader), sizeof(r_phongLightingVertexShader));

			uberShader->SetShader(ShaderStageType_Fragment, fragmentShader, "FLAG_DEFERRED FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING LIGHTING NORMAL_MAPPING PARALLAX_MAPPING SPECULAR_MAPPING");
			uberShader->SetShader(ShaderStageType_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX LIGHTING PARALLAX_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

			UberShaderLibrary::Register("PhongLighting", uberShader);
		}

		// Une fois les shaders de base enregistrés, on peut créer le matériau par défaut
		s_defaultMaterial = Material::New();
		s_defaultMaterial->Enable(RendererParameter_FaceCulling, false);
		s_defaultMaterial->SetFaceFilling(FaceFilling_Line);
		MaterialLibrary::Register("Default", s_defaultMaterial);

		return true;
	}

	void Material::Uninitialize()
	{
		s_defaultMaterial.Reset();
		UberShaderLibrary::Unregister("PhongLighting");
		UberShaderLibrary::Unregister("Basic");
		MaterialManager::Uninitialize();
		MaterialLibrary::Uninitialize();
	}

	MaterialLibrary::LibraryMap Material::s_library;
	MaterialLoader::LoaderList Material::s_loaders;
	MaterialManager::ManagerMap Material::s_managerMap;
	MaterialManager::ManagerParams Material::s_managerParameters;
	MaterialRef Material::s_defaultMaterial = nullptr;
}
