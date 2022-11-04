// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIAL_HPP
#define NAZARA_GRAPHICS_MATERIAL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Core/ResourceManager.hpp>
#include <Nazara/Core/ResourceSaver.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/MaterialSettings.hpp>
#include <Nazara/Graphics/RenderBufferPool.hpp>
#include <Nazara/Graphics/ShaderReflection.hpp>
#include <NZSL/Ast/Module.hpp>
#include <array>
#include <memory>
#include <unordered_map>

namespace Nz
{
	struct NAZARA_GRAPHICS_API MaterialParams : ResourceParameters
	{
		bool IsValid() const;
	};

	class Material;
	class MaterialInstance;
	class RenderPipelineLayout;

	using MaterialLibrary = ObjectLibrary<Material>;
	using MaterialLoader = ResourceLoader<Material, MaterialParams>;
	using MaterialManager = ResourceManager<Material, MaterialParams>;
	using MaterialSaver = ResourceSaver<Material, MaterialParams>;

	class NAZARA_GRAPHICS_API Material : public Resource, public std::enable_shared_from_this<Material>
	{
		public:
			struct TextureData;
			struct UniformBlockData;

			Material(MaterialSettings settings, const std::string& referenceModuleName);
			Material(MaterialSettings settings, const nzsl::Ast::ModulePtr& referenceModule);
			~Material() = default;

			std::shared_ptr<MaterialInstance> GetDefaultInstance() const;

			inline std::size_t FindTextureByTag(const std::string& tag) const;
			inline std::size_t FindUniformBlockByTag(const std::string& tag) const;

			inline UInt32 GetEngineBindingIndex(EngineShaderBinding shaderBinding) const;
			inline const std::shared_ptr<RenderPipelineLayout>& GetRenderPipelineLayout() const;
			inline const MaterialSettings& GetSettings() const;
			inline const TextureData& GetTextureData(std::size_t textureIndex) const;
			inline std::size_t GetTextureCount() const;
			inline const UniformBlockData& GetUniformBlockData(std::size_t uniformBlockIndex) const;
			inline std::size_t GetUniformBlockCount() const;

			std::shared_ptr<MaterialInstance> Instantiate() const;

			static std::shared_ptr<Material> Build(const ParameterList& materialData);
			static std::shared_ptr<Material> LoadFromFile(const std::filesystem::path& filePath, const MaterialParams& params = MaterialParams());
			static std::shared_ptr<Material> LoadFromMemory(const void* data, std::size_t size, const MaterialParams& params = MaterialParams());
			static std::shared_ptr<Material> LoadFromStream(Stream& stream, const MaterialParams& params = MaterialParams());

			static inline ImageType ToImageType(nzsl::ImageType imageType);

			static constexpr UInt32 InvalidBindingIndex = std::numeric_limits<UInt32>::max();
			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

			struct TextureData
			{
				UInt32 bindingSet;
				UInt32 bindingIndex;
				ImageType imageType;
			};

			struct UniformBlockData
			{
				UInt32 bindingSet;
				UInt32 bindingIndex;
				std::size_t structIndex;
				std::unique_ptr<RenderBufferPool> bufferPool;
			};

		private:
			std::array<UInt32, PredefinedShaderBindingCount> m_engineShaderBindings;
			std::shared_ptr<RenderPipelineLayout> m_renderPipelineLayout;
			std::unordered_map<UInt32, nzsl::Ast::ConstantSingleValue> m_optionValues;
			std::unordered_map<std::string /*tag*/, std::size_t> m_textureByTag;
			std::unordered_map<std::string /*tag*/, std::size_t> m_uniformBlockByTag;
			std::vector<TextureData> m_textures;
			std::vector<UniformBlockData> m_uniformBlocks;
			mutable std::weak_ptr<MaterialInstance> m_defaultInstance;
			MaterialSettings m_settings;
			ShaderReflection m_reflection;
	};
}

#include <Nazara/Graphics/Material.inl>

#endif // NAZARA_GRAPHICS_MATERIAL_HPP
