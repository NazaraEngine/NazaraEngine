// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIAL_HPP
#define NAZARA_MATERIAL_HPP

#include <Nazara/Prerequesites.hpp>
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
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Renderer/UberShader.hpp>
#include <Nazara/Utility/MaterialData.hpp>

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

	class Material;

	using MaterialConstRef = ObjectRef<const Material>;
	using MaterialLibrary = ObjectLibrary<Material>;
	using MaterialLoader = ResourceLoader<Material, MaterialParams>;
	using MaterialManager = ResourceManager<Material, MaterialParams>;
	using MaterialRef = ObjectRef<Material>;

	class NAZARA_GRAPHICS_API Material : public RefCounted, public Resource
	{
		friend MaterialLibrary;
		friend MaterialLoader;
		friend MaterialManager;
		friend class Graphics;

		public:
			inline Material();
			inline Material(const Material& material);
			inline ~Material();

			const Shader* Apply(UInt32 shaderFlags = 0, UInt8 textureUnit = 0, UInt8* lastUsedUnit = nullptr) const;

			void BuildFromParameters(const ParameterList& matData, const MaterialParams& matParams = MaterialParams());

			inline void Enable(RendererParameter renderParameter, bool enable);
			inline void EnableAlphaTest(bool alphaTest);
			inline void EnableDepthSorting(bool depthSorting);
			inline void EnableLighting(bool lighting);
			inline void EnableShadowCasting(bool castShadows);
			inline void EnableShadowReceive(bool receiveShadows);
			inline void EnableTransform(bool transform);

			inline const TextureRef& GetAlphaMap() const;
			inline float GetAlphaThreshold() const;
			inline Color GetAmbientColor() const;
			inline RendererComparison GetDepthFunc() const;
			inline const MaterialRef& GetDepthMaterial() const;
			inline Color GetDiffuseColor() const;
			inline const TextureRef& GetDiffuseMap() const;
			inline TextureSampler& GetDiffuseSampler();
			inline const TextureSampler& GetDiffuseSampler() const;
			inline BlendFunc GetDstBlend() const;
			inline const TextureRef& GetEmissiveMap() const;
			inline FaceSide GetFaceCulling() const;
			inline FaceFilling GetFaceFilling() const;
			inline const TextureRef& GetHeightMap() const;
			inline const TextureRef& GetNormalMap() const;
			inline const RenderStates& GetRenderStates() const;
			inline const UberShader* GetShader() const;
			inline const UberShaderInstance* GetShaderInstance(UInt32 flags = ShaderFlags_None) const;
			inline float GetShininess() const;
			inline Color GetSpecularColor() const;
			inline const TextureRef& GetSpecularMap() const;
			inline TextureSampler& GetSpecularSampler();
			inline const TextureSampler& GetSpecularSampler() const;
			inline BlendFunc GetSrcBlend() const;

			inline bool HasAlphaMap() const;
			inline bool HasDepthMaterial() const;
			inline bool HasDiffuseMap() const;
			inline bool HasEmissiveMap() const;
			inline bool HasHeightMap() const;
			inline bool HasNormalMap() const;
			inline bool HasSpecularMap() const;

			inline bool IsAlphaTestEnabled() const;
			inline bool IsDepthSortingEnabled() const;
			inline bool IsEnabled(RendererParameter renderParameter) const;
			inline bool IsLightingEnabled() const;
			inline bool IsShadowCastingEnabled() const;
			inline bool IsShadowReceiveEnabled() const;
			inline bool IsTransformEnabled() const;

			inline bool LoadFromFile(const String& filePath, const MaterialParams& params = MaterialParams());
			inline bool LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params = MaterialParams());
			inline bool LoadFromStream(Stream& stream, const MaterialParams& params = MaterialParams());

			void Reset();

			void SaveToParameters(ParameterList* matData);

			inline bool SetAlphaMap(const String& textureName);
			inline void SetAlphaMap(TextureRef alphaMap);
			inline void SetAlphaThreshold(float alphaThreshold);
			inline void SetAmbientColor(const Color& ambient);
			inline void SetDepthFunc(RendererComparison depthFunc);
			inline void SetDepthMaterial(MaterialRef depthMaterial);
			inline void SetDiffuseColor(const Color& diffuse);
			inline bool SetDiffuseMap(const String& textureName);
			inline void SetDiffuseMap(TextureRef diffuseMap);
			inline void SetDiffuseSampler(const TextureSampler& sampler);
			inline void SetDstBlend(BlendFunc func);
			inline bool SetEmissiveMap(const String& textureName);
			inline void SetEmissiveMap(TextureRef textureName);
			inline void SetFaceCulling(FaceSide faceSide);
			inline void SetFaceFilling(FaceFilling filling);
			inline bool SetHeightMap(const String& textureName);
			inline void SetHeightMap(TextureRef textureName);
			inline bool SetNormalMap(const String& textureName);
			inline void SetNormalMap(TextureRef textureName);
			inline void SetRenderStates(const RenderStates& states);
			inline void SetShader(UberShaderConstRef uberShader);
			inline bool SetShader(const String& uberShaderName);
			inline void SetShininess(float shininess);
			inline void SetSpecularColor(const Color& specular);
			inline bool SetSpecularMap(const String& textureName);
			inline void SetSpecularMap(TextureRef specularMap);
			inline void SetSpecularSampler(const TextureSampler& sampler);
			inline void SetSrcBlend(BlendFunc func);

			inline Material& operator=(const Material& material);

			inline static MaterialRef GetDefault();
			template<typename... Args> static MaterialRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnMaterialRelease, const Material* /*material*/);
			NazaraSignal(OnMaterialReset, const Material* /*material*/);

		private:
			struct ShaderInstance
			{
				const Shader* shader;
				UberShaderInstance* uberInstance = nullptr;
				int uniforms[MaterialUniform_Max + 1];
			};

			void Copy(const Material& material);
			void GenerateShader(UInt32 flags) const;
			inline void InvalidateShaders();

			static bool Initialize();
			static void Uninitialize();

			Color m_ambientColor;
			Color m_diffuseColor;
			Color m_specularColor;
			MaterialRef m_depthMaterial; //< Materialception
			RenderStates m_states;
			TextureSampler m_diffuseSampler;
			TextureSampler m_specularSampler;
			TextureRef m_alphaMap;
			TextureRef m_diffuseMap;
			TextureRef m_emissiveMap;
			TextureRef m_heightMap;
			TextureRef m_normalMap;
			TextureRef m_specularMap;
			UberShaderConstRef m_uberShader;
			mutable ShaderInstance m_shaders[ShaderFlags_Max + 1];
			bool m_alphaTestEnabled;
			bool m_depthSortingEnabled;
			bool m_lightingEnabled;
			bool m_shadowCastingEnabled;
			bool m_shadowReceiveEnabled;
			bool m_transformEnabled;
			float m_alphaThreshold;
			float m_shininess;

			static MaterialLibrary::LibraryMap s_library;
			static MaterialLoader::LoaderList s_loaders;
			static MaterialManager::ManagerMap s_managerMap;
			static MaterialManager::ManagerParams s_managerParameters;
			static MaterialRef s_defaultMaterial;
	};
}

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_MATERIAL_HPP
