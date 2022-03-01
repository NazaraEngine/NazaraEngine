// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSHADERMODULE_HPP
#define NAZARA_VULKANRENDERER_VULKANSHADERMODULE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/Shader/Ast/Module.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ShaderModule.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanShaderModule : public ShaderModule
	{
		public:
			struct Stage;

			VulkanShaderModule() = default;
			VulkanShaderModule(const VulkanShaderModule&) = delete;
			VulkanShaderModule(VulkanShaderModule&&) = delete;
			~VulkanShaderModule() = default;

			bool Create(Vk::Device& device, ShaderStageTypeFlags shaderStages, ShaderAst::Module& shaderModule, const ShaderWriter::States& states);
			bool Create(Vk::Device& device, ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const ShaderWriter::States& states);

			inline const Vk::ShaderModule& GetHandle() const;
			inline const std::vector<Stage>& GetStages() const;

			VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
			VulkanShaderModule& operator=(VulkanShaderModule&&) = delete;

			struct Stage
			{
				ShaderStageType stage;
				std::string name;
			};

		private:
			Vk::ShaderModule m_shaderModule;
			std::vector<Stage> m_stages;
	};
}

#include <Nazara/VulkanRenderer/VulkanShaderModule.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSHADERMODULE_HPP
