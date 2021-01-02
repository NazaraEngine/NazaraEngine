// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSHADERSTAGE_HPP
#define NAZARA_VULKANRENDERER_VULKANSHADERSTAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderStage.hpp>
#include <Nazara/Shader/ShaderWriter.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ShaderModule.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanShaderStage : public ShaderStage
	{
		public:
			VulkanShaderStage() = default;
			VulkanShaderStage(const VulkanShaderStage&) = delete;
			VulkanShaderStage(VulkanShaderStage&&) = delete;
			~VulkanShaderStage() = default;

			bool Create(Vk::Device& device, const ShaderAst& shader, const ShaderWriter::States& states);
			bool Create(Vk::Device& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize);

			inline const Vk::ShaderModule& GetHandle() const;
			inline ShaderStageType GetStageType() const;

			VulkanShaderStage& operator=(const VulkanShaderStage&) = delete;
			VulkanShaderStage& operator=(VulkanShaderStage&&) = delete;

		private:
			Vk::ShaderModule m_shaderModule;
			ShaderStageType m_stage;
	};
}

#include <Nazara/VulkanRenderer/VulkanShaderStage.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSHADERSTAGE_HPP
