// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BASE_MATERIAL_HPP
#define NAZARA_BASE_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceParameters.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialPipeline.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/UberShader.hpp>

namespace Nz
{
	struct NAZARA_GRAPHICS_API MaterialParams : ResourceParameters
	{
		bool loadAlphaMap = true;
		bool loadDiffuseMap = true;
		bool loadEmissiveMap = true;
		bool loadHeightMap = true;
		bool loadNormalMap = true;
		bool loadSpecularMap = true;
		String shaderName = "Basic";

		bool IsValid() const;
	};

	class BaseMaterial;

	using MaterialConstRef = ObjectRef<const BaseMaterial>;
	using MaterialLibrary = ObjectLibrary<BaseMaterial>;
	using MaterialLoader = ResourceLoader<BaseMaterial, MaterialParams>;
	using MaterialManager = ResourceManager<BaseMaterial, MaterialParams>;
	using MaterialRef = ObjectRef<BaseMaterial>;

	class NAZARA_GRAPHICS_API BaseMaterial : public RefCounted, public Resource
	{
		friend MaterialLibrary;
		friend MaterialLoader;
		friend MaterialManager;
		friend class Graphics;

		public:
			inline BaseMaterial();
			inline BaseMaterial(const MaterialPipeline* pipeline);
			inline BaseMaterial(const MaterialPipelineInfo& pipelineInfo);
			inline BaseMaterial(const String& pipelineName);
			inline BaseMaterial(const BaseMaterial& material);
			inline ~BaseMaterial();

			virtual void Apply(const MaterialPipeline::Instance& instance) const;

			void BuildFromParameters(const ParameterList& matData, const MaterialParams& matParams = MaterialParams());

			inline void Configure(const MaterialPipeline* pipeline);
			inline void Configure(const MaterialPipelineInfo& pipelineInfo);
			inline bool Configure(const String& pipelineName);

			inline void EnableAlphaTest(bool alphaTest);
			inline void EnableBlending(bool blending);
			inline void EnableColorWrite(bool colorWrite);
			inline void EnableDepthBuffer(bool depthBuffer);
			inline void EnableDepthSorting(bool depthSorting);
			inline void EnableDepthWrite(bool depthWrite);
			inline void EnableFaceCulling(bool faceCulling);
			inline void EnableReflectionMapping(bool reflection);
			inline void EnableScissorTest(bool scissorTest);
			inline void EnableShadowCasting(bool castShadows);
			inline void EnableShadowReceive(bool receiveShadows);
			inline void EnableStencilTest(bool stencilTest);
			inline void EnableVertexColor(bool vertexColor);

			inline void EnsurePipelineUpdate() const;

			inline float GetAlphaThreshold() const;
			inline RendererComparison GetDepthFunc() const;
			inline const MaterialRef& GetDepthMaterial() const;
			inline BlendFunc GetDstBlend() const;
			inline FaceSide GetFaceCulling() const;
			inline FaceFilling GetFaceFilling() const;
			inline float GetLineWidth() const;
			inline const MaterialPipeline* GetPipeline() const;
			inline const MaterialPipelineInfo& GetPipelineInfo() const;
			inline float GetPointSize() const;
			inline ReflectionMode GetReflectionMode() const;
			inline const UberShader* GetShader() const;
			inline BlendFunc GetSrcBlend() const;

			inline bool HasVertexColor() const;

			inline bool IsAlphaTestEnabled() const;
			inline bool IsBlendingEnabled() const;
			inline bool IsColorWriteEnabled() const;
			inline bool IsDepthBufferEnabled() const;
			inline bool IsDepthSortingEnabled() const;
			inline bool IsDepthWriteEnabled() const;
			inline bool IsFaceCullingEnabled() const;
			inline bool IsReflectionMappingEnabled() const;
			inline bool IsScissorTestEnabled() const;
			inline bool IsStencilTestEnabled() const;
			inline bool IsShadowCastingEnabled() const;
			inline bool IsShadowReceiveEnabled() const;

			void Reset();

			virtual void SaveToParameters(ParameterList* matData) = 0;

			inline void SetDepthFunc(RendererComparison depthFunc);
			inline void SetDepthMaterial(MaterialRef depthMaterial);
			inline void SetDstBlend(BlendFunc func);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline void SetLineWidth(float lineWidth);
			inline void SetPointSize(float pointSize);
			inline void SetReflectionMode(ReflectionMode reflectionMode);
			inline void SetShader(UberShaderConstRef uberShader);
			inline bool SetShader(const String& uberShaderName);
			inline void SetSpecularSampler(const TextureSampler& sampler);
			inline void SetSrcBlend(BlendFunc func);

			inline BaseMaterial& operator=(const BaseMaterial& material);

			inline static MaterialRef GetDefault();
			inline static int GetTextureUnit(TextureMap textureMap);

			static inline MaterialRef LoadFromFile(const String& filePath, const MaterialParams& params = MaterialParams());
			static inline MaterialRef LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params = MaterialParams());
			static inline MaterialRef LoadFromStream(Stream& stream, const MaterialParams& params = MaterialParams());

			template<typename... Args> static MaterialRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnMaterialReflectionModeChange, const BaseMaterial* /*material*/, ReflectionMode /*newReflectionMode*/);
			NazaraSignal(OnMaterialRelease, const BaseMaterial* /*material*/);
			NazaraSignal(OnMaterialReset, const BaseMaterial* /*material*/);

		protected:
			void Copy(const BaseMaterial& material);
			inline void InvalidatePipeline();
			inline void UpdatePipeline() const;

			static bool Initialize();
			static void Uninitialize();

			MaterialRef m_depthMaterial; //< Materialception
			mutable const MaterialPipeline* m_pipeline;
			MaterialPipelineInfo m_pipelineInfo;
			ReflectionMode m_reflectionMode;
			mutable bool m_pipelineUpdated;
			bool m_shadowCastingEnabled;
			unsigned int m_reflectionSize;

			static std::array<int, TextureMap_Max + 1> s_textureUnits;
			static MaterialLibrary::LibraryMap s_library;
			static MaterialLoader::LoaderList s_loaders;
			static MaterialManager::ManagerMap s_managerMap;
			static MaterialManager::ManagerParams s_managerParameters;
			static MaterialRef s_defaultMaterial;
	};
}

#include <Nazara/Graphics/BaseMaterial.inl>

#endif // NAZARA_BASE_MATERIAL_HPP
