// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANSHADERMODULE_HPP
#define NAZARA_VULKANRENDERER_VULKANSHADERMODULE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/VulkanRenderer/Wrapper/ShaderModule.hpp>
#include <NZSL/BackendParameters.hpp>
#include <NZSL/Ast/Module.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanShaderModule : public ShaderModule
	{
		public:
			struct Stage;

			bool Create(Vk::Device& device, nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::Module&& shaderModule, const nzsl::BackendParameters& parameters);
			bool Create(Vk::Device& device, nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::BackendParameters& parameters);

			inline const Vk::ShaderModule& GetHandle() const;
			inline const std::vector<Stage>& GetStages() const;

			void UpdateDebugName(std::string_view name) override;

			struct Stage
			{
				nzsl::ShaderStageType stage;
				std::string name;
			};

		private:
			Vk::ShaderModule m_shaderModule;
			std::vector<Stage> m_stages;
	};
}

#include <Nazara/VulkanRenderer/VulkanShaderModule.inl>

#endif // NAZARA_VULKANRENDERER_VULKANSHADERMODULE_HPP
