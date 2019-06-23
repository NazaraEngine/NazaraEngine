// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATERIALPIPELINESETTINGS_HPP
#define NAZARA_MATERIALPIPELINESETTINGS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <limits>
#include <string>
#include <vector>

namespace Nz
{
	class MaterialSettings
	{
		public:
			struct SharedUniformBlocks;
			struct Texture;
			struct UniformBlocks;

			inline MaterialSettings();
			inline MaterialSettings(std::vector<Texture> textures, std::vector<UniformBlocks> uniformBlocks, std::vector<SharedUniformBlocks> sharedUniformBlocks);
			MaterialSettings(const MaterialSettings&) = default;
			MaterialSettings(MaterialSettings&&) = delete;
			~MaterialSettings() = default;

			inline const RenderPipelineLayoutRef& GetRenderPipelineLayout() const;
			inline const std::vector<SharedUniformBlocks>& GetSharedUniformBlocks() const;
			inline std::size_t GetSharedUniformBlockVariableOffset(std::size_t uniformBlockIndex, const String& name) const;
			inline std::size_t GetSharedUniformBlockIndex(const String& name) const;
			inline const std::vector<Texture>& GetTextures() const;
			inline std::size_t GetTextureIndex(const String& name) const;
			inline const std::vector<UniformBlocks>& GetUniformBlocks() const;
			inline std::size_t GetUniformBlockIndex(const String& name) const;
			inline std::size_t GetUniformBlockVariableOffset(std::size_t uniformBlockIndex, const String& name) const;

			MaterialSettings& operator=(const MaterialSettings&) = delete;
			MaterialSettings& operator=(MaterialSettings&&) = delete;

			static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

			struct UniformVariable
			{
				String name;
				std::size_t offset;
			};

			struct SharedUniformBlocks
			{
				String name;
				std::string bindingPoint;
				std::vector<UniformVariable> uniforms;
			};

			struct Texture
			{
				String name;
				ImageType type;
				std::string bindingPoint;
			};

			struct UniformBlocks
			{
				String name;
				std::size_t blockSize;
				std::string bindingPoint;
				std::vector<UniformVariable> uniforms;
			};

		private:
			std::vector<SharedUniformBlocks> m_sharedUniformBlocks;
			std::vector<Texture> m_textures;
			std::vector<UniformBlocks> m_uniformBlocks;
			RenderPipelineLayoutRef m_pipelineLayout;
	};
}

#include <Nazara/Graphics/MaterialPipelineSettings.inl>

#endif // NAZARA_MATERIALPIPELINESETTINGS_HPP
