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
			Material();
			Material(const Material& material);
			~Material();

			const Shader* Apply(UInt32 shaderFlags = 0, UInt8 textureUnit = 0, UInt8* lastUsedUnit = nullptr) const;

			void Enable(RendererParameter renderParameter, bool enable);
			void EnableAlphaTest(bool alphaTest);
			void EnableDepthSorting(bool depthSorting);
			void EnableLighting(bool lighting);
			void EnableTransform(bool transform);

			Texture* GetAlphaMap() const;
			float GetAlphaThreshold() const;
			Color GetAmbientColor() const;
			RendererComparison GetDepthFunc() const;
			Color GetDiffuseColor() const;
			Texture* GetDiffuseMap() const;
			TextureSampler& GetDiffuseSampler();
			const TextureSampler& GetDiffuseSampler() const;
			BlendFunc GetDstBlend() const;
			Texture* GetEmissiveMap() const;
			FaceSide GetFaceCulling() const;
			FaceFilling GetFaceFilling() const;
			Texture* GetHeightMap() const;
			Texture* GetNormalMap() const;
			const RenderStates& GetRenderStates() const;
			const UberShader* GetShader() const;
			const UberShaderInstance* GetShaderInstance(UInt32 flags = ShaderFlags_None) const;
			float GetShininess() const;
			Color GetSpecularColor() const;
			Texture* GetSpecularMap() const;
			TextureSampler& GetSpecularSampler();
			const TextureSampler& GetSpecularSampler() const;
			BlendFunc GetSrcBlend() const;

			bool HasAlphaMap() const;
			bool HasDiffuseMap() const;
			bool HasEmissiveMap() const;
			bool HasHeightMap() const;
			bool HasNormalMap() const;
			bool HasSpecularMap() const;

			bool IsAlphaTestEnabled() const;
			bool IsDepthSortingEnabled() const;
			bool IsEnabled(RendererParameter renderParameter) const;
			bool IsLightingEnabled() const;
			bool IsTransformEnabled() const;

			bool LoadFromFile(const String& filePath, const MaterialParams& params = MaterialParams());
			bool LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params = MaterialParams());
			bool LoadFromStream(Stream& stream, const MaterialParams& params = MaterialParams());

			void Reset();

			bool SetAlphaMap(const String& textureName);
			void SetAlphaMap(TextureRef alphaMap);
			void SetAlphaThreshold(float alphaThreshold);
			void SetAmbientColor(const Color& ambient);
			void SetDepthFunc(RendererComparison depthFunc);
			void SetDiffuseColor(const Color& diffuse);
			bool SetDiffuseMap(const String& textureName);
			void SetDiffuseMap(TextureRef diffuseMap);
			void SetDiffuseSampler(const TextureSampler& sampler);
			void SetDstBlend(BlendFunc func);
			bool SetEmissiveMap(const String& textureName);
			void SetEmissiveMap(TextureRef textureName);
			void SetFaceCulling(FaceSide faceSide);
			void SetFaceFilling(FaceFilling filling);
			bool SetHeightMap(const String& textureName);
			void SetHeightMap(TextureRef textureName);
			bool SetNormalMap(const String& textureName);
			void SetNormalMap(TextureRef textureName);
			void SetRenderStates(const RenderStates& states);
			void SetShader(UberShaderConstRef uberShader);
			bool SetShader(const String& uberShaderName);
			void SetShininess(float shininess);
			void SetSpecularColor(const Color& specular);
			bool SetSpecularMap(const String& textureName);
			void SetSpecularMap(TextureRef specularMap);
			void SetSpecularSampler(const TextureSampler& sampler);
			void SetSrcBlend(BlendFunc func);

			Material& operator=(const Material& material);

			static MaterialRef GetDefault();
			template<typename... Args> static MaterialRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnMaterialRelease, const Material* /*material*/);
			NazaraSignal(OnMaterialReset, const Material* /*material*/);

		private:
			struct ShaderInstance
			{
				const Shader* shader;
				UberShaderInstance* uberInstance = nullptr;
				int uniforms[MaterialUniform_Max+1];
			};

			void Copy(const Material& material);
			void GenerateShader(UInt32 flags) const;
			void InvalidateShaders();

			static bool Initialize();
			static void Uninitialize();

			Color m_ambientColor;
			Color m_diffuseColor;
			Color m_specularColor;
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
			mutable ShaderInstance m_shaders[ShaderFlags_Max+1];
			bool m_alphaTestEnabled;
			bool m_depthSortingEnabled;
			bool m_lightingEnabled;
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
