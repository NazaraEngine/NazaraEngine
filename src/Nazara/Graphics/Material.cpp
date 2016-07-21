// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Mandatory to include the OpenGL headers
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

	/*!
	* \ingroup graphics
	* \class Nz::Material
	* \brief Graphics class that represents a material
	*/

	/*!
	* \brief Checks whether the parameters for the material are correct
	* \return true If parameters are valid
	*/

	bool MaterialParams::IsValid() const
	{
		if (!UberShaderLibrary::Has(shaderName))
			return false;

		return true;
	}

	/*!
	* \brief Applies shader to the material
	* \return Constant pointer to the shader
	*
	* \param shaderFlags Flags for the shader
	* \param textureUnit Unit for the texture GL_TEXTURE"i"
	* \param lastUsedUnit Optional argument to get the last texture unit
	*/

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

	/*!
	* \brief Builds the material from parameters
	*
	* \param matData Data information for the material
	* \param matParams Parameters for the material
	*/

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

		if (matData.GetIntegerParameter(MaterialData::CullingSide, &iValue))
			SetFaceCulling(static_cast<FaceSide>(iValue));

		if (matData.GetIntegerParameter(MaterialData::DepthFunc, &iValue))
			SetDepthFunc(static_cast<RendererComparison>(iValue));

		if (matData.GetBooleanParameter(MaterialData::DepthSorting, &isEnabled))
			EnableDepthSorting(isEnabled);

		if (matData.GetColorParameter(MaterialData::DiffuseColor, &color))
			SetDiffuseColor(color);

		if (matData.GetIntegerParameter(MaterialData::DstBlend, &iValue))
			SetDstBlend(static_cast<BlendFunc>(iValue));

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
			m_states.stencilCompare.front = static_cast<RendererComparison>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilFail, &iValue))
			m_states.stencilFail.front = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilPass, &iValue))
			m_states.stencilPass.front = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilZFail, &iValue))
			m_states.stencilDepthFail.front = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilMask, &iValue))
			m_states.stencilWriteMask.front = static_cast<UInt32>(iValue);

		if (matData.GetIntegerParameter(MaterialData::StencilReference, &iValue))
			m_states.stencilReference.front = static_cast<unsigned int>(iValue);

		// Stencil (back)
		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilCompare, &iValue))
			m_states.stencilCompare.back = static_cast<RendererComparison>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilFail, &iValue))
			m_states.stencilFail.back = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilPass, &iValue))
			m_states.stencilPass.back = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilZFail, &iValue))
			m_states.stencilDepthFail.back = static_cast<StencilOperation>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilMask, &iValue))
			m_states.stencilWriteMask.back = static_cast<UInt32>(iValue);

		if (matData.GetIntegerParameter(MaterialData::BackFaceStencilReference, &iValue))
			m_states.stencilReference.back = static_cast<unsigned int>(iValue);

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

	void Material::SaveToParameters(ParameterList* matData)
	{
		NazaraAssert(matData, "Invalid ParameterList");

		matData->SetParameter(MaterialData::AlphaTest, IsAlphaTestEnabled());
		matData->SetParameter(MaterialData::AlphaThreshold, GetAlphaThreshold());
		matData->SetParameter(MaterialData::AmbientColor, GetAmbientColor());
		matData->SetParameter(MaterialData::CullingSide, int(GetFaceCulling()));
		matData->SetParameter(MaterialData::DepthFunc, int(GetDepthFunc()));
		matData->SetParameter(MaterialData::DepthSorting, IsDepthSortingEnabled());
		matData->SetParameter(MaterialData::DiffuseColor, GetDiffuseColor());
		matData->SetParameter(MaterialData::DstBlend, int(GetDstBlend()));
		matData->SetParameter(MaterialData::FaceFilling, int(GetFaceFilling()));
		matData->SetParameter(MaterialData::Lighting, IsLightingEnabled());
		matData->SetParameter(MaterialData::LineWidth, GetRenderStates().lineWidth);
		matData->SetParameter(MaterialData::PointSize, GetRenderStates().pointSize);
		matData->SetParameter(MaterialData::Shininess, GetShininess());
		matData->SetParameter(MaterialData::SpecularColor, GetSpecularColor());
		matData->SetParameter(MaterialData::SrcBlend, int(GetSrcBlend()));
		matData->SetParameter(MaterialData::Transform, IsTransformEnabled());

		// RendererParameter
		matData->SetParameter(MaterialData::Blending, GetRenderStates().blending);
		matData->SetParameter(MaterialData::ColorWrite, GetRenderStates().colorWrite);
		matData->SetParameter(MaterialData::DepthBuffer, GetRenderStates().depthBuffer);
		matData->SetParameter(MaterialData::DepthWrite, GetRenderStates().depthWrite);
		matData->SetParameter(MaterialData::FaceCulling, GetRenderStates().faceCulling);
		matData->SetParameter(MaterialData::ScissorTest, GetRenderStates().scissorTest);
		matData->SetParameter(MaterialData::StencilTest, GetRenderStates().stencilTest);

		// Samplers
		matData->SetParameter(MaterialData::DiffuseAnisotropyLevel, int(GetDiffuseSampler().GetAnisotropicLevel()));
		matData->SetParameter(MaterialData::DiffuseFilter, int(GetDiffuseSampler().GetFilterMode()));
		matData->SetParameter(MaterialData::DiffuseWrap, int(GetDiffuseSampler().GetWrapMode()));

		matData->SetParameter(MaterialData::SpecularAnisotropyLevel, int(GetSpecularSampler().GetAnisotropicLevel()));
		matData->SetParameter(MaterialData::SpecularFilter, int(GetSpecularSampler().GetFilterMode()));
		matData->SetParameter(MaterialData::SpecularWrap, int(GetSpecularSampler().GetWrapMode()));

		// Stencil
		matData->SetParameter(MaterialData::StencilCompare, int(GetRenderStates().stencilCompare.front));
		matData->SetParameter(MaterialData::StencilFail, int(GetRenderStates().stencilFail.front));
		matData->SetParameter(MaterialData::StencilPass, int(GetRenderStates().stencilPass.front));
		matData->SetParameter(MaterialData::StencilZFail, int(GetRenderStates().stencilDepthFail.front));
		matData->SetParameter(MaterialData::StencilMask, int(GetRenderStates().stencilWriteMask.front));
		matData->SetParameter(MaterialData::StencilReference, int(GetRenderStates().stencilReference.front));

		// Stencil (back)
		matData->SetParameter(MaterialData::BackFaceStencilCompare, int(GetRenderStates().stencilCompare.back));
		matData->SetParameter(MaterialData::BackFaceStencilFail, int(GetRenderStates().stencilFail.back));
		matData->SetParameter(MaterialData::BackFaceStencilPass, int(GetRenderStates().stencilPass.back));
		matData->SetParameter(MaterialData::BackFaceStencilZFail, int(GetRenderStates().stencilDepthFail.back));
		matData->SetParameter(MaterialData::BackFaceStencilMask, int(GetRenderStates().stencilWriteMask.back));
		matData->SetParameter(MaterialData::BackFaceStencilReference, int(GetRenderStates().stencilReference.back));

		// Textures
		if (HasAlphaMap())
		{
			const String& path = GetAlphaMap()->GetFilePath();
			if (!path.IsEmpty())
				matData->SetParameter(MaterialData::AlphaTexturePath, path);
		}

		if (HasDiffuseMap())
		{
			const String& path = GetDiffuseMap()->GetFilePath();
			if (!path.IsEmpty())
				matData->SetParameter(MaterialData::DiffuseTexturePath, path);
		}

		if (HasEmissiveMap())
		{
			const String& path = GetEmissiveMap()->GetFilePath();
			if (!path.IsEmpty())
				matData->SetParameter(MaterialData::EmissiveTexturePath, path);
		}

		if (HasHeightMap())
		{
			const String& path = GetHeightMap()->GetFilePath();
			if (!path.IsEmpty())
				matData->SetParameter(MaterialData::HeightTexturePath, path);
		}

		if (HasNormalMap())
		{
			const String& path = GetNormalMap()->GetFilePath();
			if (!path.IsEmpty())
				matData->SetParameter(MaterialData::NormalTexturePath, path);
		}

		if (HasSpecularMap())
		{
			const String& path = GetSpecularMap()->GetFilePath();
			if (!path.IsEmpty())
				matData->SetParameter(MaterialData::SpecularTexturePath, path);
		}
	}

	/*!
	* \brief Resets the material, cleans everything
	*/
	void Material::Reset()
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
		m_ambientColor = Color(128, 128, 128);
		m_depthSortingEnabled = false;
		m_diffuseColor = Color::White;
		m_diffuseSampler = TextureSampler();
		m_lightingEnabled = true;
		m_shadowCastingEnabled = true;
		m_shadowReceiveEnabled = true;
		m_shininess = 50.f;
		m_specularColor = Color::White;
		m_specularSampler = TextureSampler();
		m_states = RenderStates();
		m_states.depthBuffer = true;
		m_states.faceCulling = true;
		m_transformEnabled = true;

		SetShader("Basic");
	}

	/*!
	* \brief Copies the other material
	*
	* \param material Material to copy into this
	*/

	void Material::Copy(const Material& material)
	{
		// Copy of base states
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

		// Copy of reference to the textures
		m_alphaMap      = material.m_alphaMap;
		m_depthMaterial = material.m_depthMaterial;
		m_diffuseMap    = material.m_diffuseMap;
		m_emissiveMap   = material.m_emissiveMap;
		m_heightMap     = material.m_heightMap;
		m_normalMap     = material.m_normalMap;
		m_specularMap   = material.m_specularMap;
		m_uberShader = material.m_uberShader;

		// We copy the instances of the shader too
		std::memcpy(&m_shaders[0], &material.m_shaders[0], (ShaderFlags_Max + 1) * sizeof(ShaderInstance));
	}

	/*!
	* \brief Generates the shader based on flag
	*
	* \param flags Flag for the shaer
	*/

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
		list.SetParameter("SHADOW_MAPPING", m_shadowReceiveEnabled);
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

	/*!
	* \brief Initializes the material librairies
	* \return true If successful
	*
	* \remark Produces a NazaraError if the material library failed to be initialized
	*/

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

			uberShader->SetShader(ShaderStageType_Fragment, fragmentShader, "FLAG_DEFERRED FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING LIGHTING NORMAL_MAPPING PARALLAX_MAPPING SHADOW_MAPPING SPECULAR_MAPPING");
			uberShader->SetShader(ShaderStageType_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX LIGHTING PARALLAX_MAPPING SHADOW_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

			UberShaderLibrary::Register("PhongLighting", uberShader);
		}

		// Once the base shaders are registered, we can now set some default materials
		s_defaultMaterial = New();
		s_defaultMaterial->Enable(RendererParameter_FaceCulling, false);
		s_defaultMaterial->SetFaceFilling(FaceFilling_Line);
		MaterialLibrary::Register("Default", s_defaultMaterial);

		MaterialRef mat;

		mat = New();
		mat->Enable(RendererParameter_DepthWrite, false);
		mat->Enable(RendererParameter_FaceCulling, false);
		mat->EnableLighting(false);
		MaterialLibrary::Register("Basic2D", std::move(mat));

		mat = New();
		mat->Enable(RendererParameter_Blend, true);
		mat->Enable(RendererParameter_DepthWrite, false);
		mat->Enable(RendererParameter_FaceCulling, false);
		mat->EnableLighting(false);
		mat->SetDstBlend(BlendFunc_InvSrcAlpha);
		mat->SetSrcBlend(BlendFunc_SrcAlpha);
		MaterialLibrary::Register("Translucent2D", std::move(mat));

		return true;
	}

	/*!
	* \brief Uninitializes the material librairies
	*/

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
