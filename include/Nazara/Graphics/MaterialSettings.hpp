// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALSETTINGS_HPP
#define NAZARA_GRAPHICS_MATERIALSETTINGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/UberShader.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <array>
#include <limits>
#include <string>
#include <vector>

namespace Nz
{
	class MaterialSettings
	{
		public:
			struct Builder;
			struct Option;
			struct SharedUniformBlock;
			struct Texture;
			struct UniformBlock;
			using PredefinedBinding = std::array<std::size_t, PredefinedShaderBindingCount>;

			inline MaterialSettings();
			inline MaterialSettings(Builder builder);
			MaterialSettings(const MaterialSettings&) = default;
			MaterialSettings(MaterialSettings&&) = delete;
			~MaterialSettings() = default;

			inline const Builder& GetBuilderData() const;
			inline const std::vector<Option>& GetOptions() const;
			inline std::size_t GetOptionIndex(const std::string_view& name) const;
			inline std::size_t GetPredefinedBinding(PredefinedShaderBinding shaderBinding) const;
			inline const std::shared_ptr<RenderPipelineLayout>& GetRenderPipelineLayout() const;
			inline const std::shared_ptr<UberShader>& GetShader(ShaderStageType stage) const;
			inline const std::vector<std::shared_ptr<UberShader>>& GetShaders() const;
			inline const std::vector<SharedUniformBlock>& GetSharedUniformBlocks() const;
			inline std::size_t GetSharedUniformBlockIndex(const std::string_view& name) const;
			inline std::size_t GetSharedUniformBlockVariableOffset(std::size_t uniformBlockIndex, const std::string_view& name) const;
			inline const std::vector<Texture>& GetTextures() const;
			inline std::size_t GetTextureIndex(const std::string_view& name) const;
			inline const std::vector<UniformBlock>& GetUniformBlocks() const;
			inline std::size_t GetUniformBlockIndex(const std::string_view& name) const;
			inline std::size_t GetUniformBlockVariableOffset(std::size_t uniformBlockIndex, const std::string_view& name) const;

			MaterialSettings& operator=(const MaterialSettings&) = delete;
			MaterialSettings& operator=(MaterialSettings&&) = delete;

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

			static inline void BuildOption(std::vector<Option>& options, const std::vector<std::shared_ptr<UberShader>>& uberShaders, std::string optionName, const std::string& shaderOptionName);

			struct Builder
			{
				inline Builder();

				std::vector<std::shared_ptr<UberShader>> shaders;
				std::vector<Option> options;
				std::vector<Texture> textures;
				std::vector<UniformBlock> uniformBlocks;
				std::vector<SharedUniformBlock> sharedUniformBlocks;
				PredefinedBinding predefinedBindings;
			};

			struct Option
			{
				std::string name;
				std::vector<std::optional<std::size_t>> optionIndexByShader;
			};

			struct UniformVariable
			{
				std::string name;
				std::size_t offset;
			};

			struct SharedUniformBlock
			{
				UInt32 bindingIndex;
				std::string name;
				std::vector<UniformVariable> uniforms;
				ShaderStageTypeFlags shaderStages = ShaderStageType_All;
			};

			struct Texture
			{
				UInt32 bindingIndex;
				std::string name;
				ImageType type;
				ShaderStageTypeFlags shaderStages = ShaderStageType_All;
			};

			struct UniformBlock
			{
				UInt32 bindingIndex;
				std::string name;
				std::size_t blockSize;
				std::vector<UniformVariable> uniforms;
				std::vector<UInt8> defaultValues;
				ShaderStageTypeFlags shaderStages = ShaderStageType_All;
			};

		private:
			std::shared_ptr<RenderPipelineLayout> m_pipelineLayout;
			Builder m_data;
	};
}

#include <Nazara/Graphics/MaterialSettings.inl>

#endif // NAZARA_GRAPHICS_MATERIALSETTINGS_HPP
