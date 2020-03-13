// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSHADERSTAGE_HPP
#define NAZARA_VULKANRENDERER_VULKANSHADERSTAGE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderStageImpl.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ShaderModule.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanShaderStage : public ShaderStageImpl
	{
		public:
			VulkanShaderStage() = default;
			VulkanShaderStage(const VulkanShaderStage&) = delete;
			VulkanShaderStage(VulkanShaderStage&&) noexcept = default;
			~VulkanShaderStage() = default;

			bool Create(Vk::Device& device, ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize);

			inline const Vk::ShaderModule& GetHandle() const;
			inline ShaderStageType GetStageType() const;

			VulkanShaderStage& operator=(const VulkanShaderStage&) = delete;
			VulkanShaderStage& operator=(VulkanShaderStage&&) noexcept = default;

		private:
			Vk::ShaderModule m_shaderModule;
			ShaderStageType m_stage;
	};
}

#include <Nazara/VulkanRenderer/VulkanShaderStage.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSHADERSTAGE_HPP
