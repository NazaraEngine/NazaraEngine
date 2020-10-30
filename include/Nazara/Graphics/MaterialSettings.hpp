// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIALSETTINGS_HPP
#define NAZARA_MATERIALSETTINGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
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
			using DefaultShaders = std::array<std::shared_ptr<ShaderStage>, ShaderStageTypeCount>;
			using PredefinedBinding = std::array<std::size_t, PredefinedShaderBinding_Max + 1>;

			struct SharedUniformBlock;
			struct Texture;
			struct UniformBlock;

			inline MaterialSettings();
			inline MaterialSettings(std::vector<Texture> textures, std::vector<UniformBlock> uniformBlocks, std::vector<SharedUniformBlock> sharedUniformBlocks, const PredefinedBinding& predefinedBinding, DefaultShaders defaultShaders);
			MaterialSettings(const MaterialSettings&) = default;
			MaterialSettings(MaterialSettings&&) = delete;
			~MaterialSettings() = default;

			inline const std::shared_ptr<ShaderStage>& GetDefaultShader(ShaderStageType stage) const;
			inline const DefaultShaders& GetDefaultShaders() const;
			inline std::size_t GetPredefinedBindingIndex(PredefinedShaderBinding binding) const;
			inline const std::shared_ptr<RenderPipelineLayout>& GetRenderPipelineLayout() const;
			inline const std::vector<SharedUniformBlock>& GetSharedUniformBlocks() const;
			inline std::size_t GetSharedUniformBlockVariableOffset(std::size_t uniformBlockIndex, const std::string_view& name) const;
			inline std::size_t GetSharedUniformBlockIndex(const std::string_view& name) const;
			inline const std::vector<Texture>& GetTextures() const;
			inline std::size_t GetTextureIndex(const std::string_view& name) const;
			inline const std::vector<UniformBlock>& GetUniformBlocks() const;
			inline std::size_t GetUniformBlockIndex(const std::string_view& name) const;
			inline std::size_t GetUniformBlockVariableOffset(std::size_t uniformBlockIndex, const std::string_view& name) const;

			MaterialSettings& operator=(const MaterialSettings&) = delete;
			MaterialSettings& operator=(MaterialSettings&&) = delete;

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

			struct UniformVariable
			{
				std::string name;
				std::size_t offset;
			};

			struct SharedUniformBlock
			{
				std::string name;
				std::string bindingPoint;
				std::vector<UniformVariable> uniforms;
			};

			struct Texture
			{
				std::string bindingPoint;
				std::string name;
				ImageType type;
			};

			struct UniformBlock
			{
				std::size_t blockSize;
				std::string name;
				std::string bindingPoint;
				std::vector<UniformVariable> uniforms;
				std::vector<UInt8> defaultValues;
			};

		private:
			std::shared_ptr<RenderPipelineLayout> m_pipelineLayout;
			std::vector<SharedUniformBlock> m_sharedUniformBlocks;
			std::vector<Texture> m_textures;
			std::vector<UniformBlock> m_uniformBlocks;
			DefaultShaders m_defaultShaders;
			PredefinedBinding m_predefinedBinding;
	};
}

#include <Nazara/Graphics/MaterialSettings.inl>

#endif // NAZARA_MATERIALPIPELINESETTINGS_HPP
