// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/MaterialPipeline.hpp>

#ifndef NAZARA_RENDERER_OPENGL
#define NAZARA_RENDERER_OPENGL // Mandatory to include the OpenGL headers
#endif

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/UberShaderPreprocessor.hpp>
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
	* \class Nz::MaterialPipeline
	*
	* \brief Graphics class used to contains all rendering states that are not allowed to change individually on rendering devices
	*/

	/*!
	* \brief Returns a reference to a MaterialPipeline built with MaterialPipelineInfo
	*
	* This function is using a cache, calling it multiples times with the same MaterialPipelineInfo will returns references to a single MaterialPipeline
	*
	* \param pipelineInfo Pipeline informations used to build/retrieve a MaterialPipeline object
	*/
	MaterialPipelineRef MaterialPipeline::GetPipeline(const MaterialPipelineInfo& pipelineInfo)
	{
		auto it = s_pipelineCache.lower_bound(pipelineInfo);
		if (it == s_pipelineCache.end() || it->first != pipelineInfo)
			it = s_pipelineCache.insert(it, PipelineCache::value_type(pipelineInfo, New(pipelineInfo)));

		return it->second;
	}

	void MaterialPipeline::GenerateRenderPipeline(UInt32 flags) const
	{
		ParameterList list;
		list.SetParameter("ALPHA_MAPPING",     m_pipelineInfo.hasAlphaMap);
		list.SetParameter("ALPHA_TEST",        m_pipelineInfo.alphaTest);
		list.SetParameter("COMPUTE_TBNMATRIX", m_pipelineInfo.hasNormalMap || m_pipelineInfo.hasHeightMap);
		list.SetParameter("DIFFUSE_MAPPING",   m_pipelineInfo.hasDiffuseMap);
		list.SetParameter("EMISSIVE_MAPPING",  m_pipelineInfo.hasEmissiveMap);
		list.SetParameter("NORMAL_MAPPING",    m_pipelineInfo.hasNormalMap);
		list.SetParameter("PARALLAX_MAPPING",  m_pipelineInfo.hasHeightMap);
		list.SetParameter("SHADOW_MAPPING",    m_pipelineInfo.shadowReceive);
		list.SetParameter("SPECULAR_MAPPING",  m_pipelineInfo.hasSpecularMap);
		list.SetParameter("TEXTURE_MAPPING",   m_pipelineInfo.hasAlphaMap  || m_pipelineInfo.hasDiffuseMap || m_pipelineInfo.hasEmissiveMap ||
		                                       m_pipelineInfo.hasNormalMap || m_pipelineInfo.hasHeightMap  || m_pipelineInfo.hasSpecularMap ||
		                                       flags & ShaderFlags_TextureOverlay);
		list.SetParameter("TRANSFORM",         true);

		list.SetParameter("FLAG_BILLBOARD",      static_cast<bool>((flags & ShaderFlags_Billboard) != 0));
		list.SetParameter("FLAG_DEFERRED",       static_cast<bool>((flags & ShaderFlags_Deferred) != 0));
		list.SetParameter("FLAG_INSTANCING",     static_cast<bool>((flags & ShaderFlags_Instancing) != 0));
		list.SetParameter("FLAG_TEXTUREOVERLAY", static_cast<bool>((flags & ShaderFlags_TextureOverlay) != 0));
		list.SetParameter("FLAG_VERTEXCOLOR",    static_cast<bool>((flags & ShaderFlags_VertexColor) != 0));

		Instance& instance = m_instances[flags];
		instance.uberInstance = m_pipelineInfo.uberShader->Get(list);

		RenderPipelineInfo renderPipelineInfo;
		static_cast<RenderStates&>(renderPipelineInfo).operator=(m_pipelineInfo); // Not my proudest line

		renderPipelineInfo.shader = instance.uberInstance->GetShader();

		instance.renderPipeline.Create(renderPipelineInfo);

		#define CacheUniform(name) instance.uniforms[MaterialUniform_##name] = renderPipelineInfo.shader->GetUniformLocation("Material" #name)

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

	bool MaterialPipeline::Initialize()
	{
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

			uberShader->SetShader(ShaderStageType_Fragment, fragmentShader, "FLAG_DEFERRED FLAG_TEXTUREOVERLAY ALPHA_MAPPING ALPHA_TEST AUTO_TEXCOORDS DIFFUSE_MAPPING EMISSIVE_MAPPING NORMAL_MAPPING PARALLAX_MAPPING SHADOW_MAPPING SPECULAR_MAPPING");
			uberShader->SetShader(ShaderStageType_Vertex, vertexShader, "FLAG_BILLBOARD FLAG_DEFERRED FLAG_INSTANCING FLAG_VERTEXCOLOR COMPUTE_TBNMATRIX PARALLAX_MAPPING SHADOW_MAPPING TEXTURE_MAPPING TRANSFORM UNIFORM_VERTEX_DEPTH");

			UberShaderLibrary::Register("PhongLighting", uberShader);
		}

		// Once the base shaders are registered, we can now set some default materials
		MaterialPipelineInfo pipelineInfo;

		// Basic 2D - No depth write/face culling
		pipelineInfo.depthWrite = false;
		pipelineInfo.faceCulling = false;

		MaterialPipelineLibrary::Register("Basic2D", GetPipeline(pipelineInfo));

		// Translucent 2D - Alpha blending with no depth write/face culling
		pipelineInfo.blending = false;
		pipelineInfo.depthWrite = false;
		pipelineInfo.faceCulling = false;
		pipelineInfo.dstBlend = BlendFunc_InvSrcAlpha;
		pipelineInfo.srcBlend = BlendFunc_SrcAlpha;

		MaterialPipelineLibrary::Register("Translucent2D", GetPipeline(pipelineInfo));

		return true;
	}

	void MaterialPipeline::Uninitialize()
	{
		s_pipelineCache.clear();
		UberShaderLibrary::Unregister("PhongLighting");
		UberShaderLibrary::Unregister("Basic");
		MaterialPipelineLibrary::Uninitialize();
	}

	MaterialPipelineLibrary::LibraryMap MaterialPipeline::s_library;
	MaterialPipeline::PipelineCache MaterialPipeline::s_pipelineCache;
}