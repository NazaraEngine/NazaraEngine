// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_UBERSHADER_HPP
#define NAZARA_GRAPHICS_UBERSHADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <NazaraUtils/Signal.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <NZSL/ModuleResolver.hpp>
#include <NZSL/Ast/Module.hpp>
#include <NZSL/Ast/Option.hpp>
#include <unordered_map>
#include <unordered_set>

namespace Nz
{
	class ShaderModule;

	class NAZARA_GRAPHICS_API UberShader
	{
		public:
			struct Config;
			struct Option;
			using ConfigCallback = std::function<void(Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers)>;

			UberShader(nzsl::ShaderStageTypeFlags shaderStages, std::string moduleName);
			UberShader(nzsl::ShaderStageTypeFlags shaderStages, nzsl::ModuleResolver& moduleResolver, std::string moduleName);
			UberShader(nzsl::ShaderStageTypeFlags shaderStages, nzsl::Ast::ModulePtr shaderModule);
			~UberShader() = default;

			inline nzsl::ShaderStageTypeFlags GetSupportedStages() const;

			const std::shared_ptr<ShaderModule>& Get(const Config& config);

			inline bool HasOption(std::string_view optionName, Pointer<const Option>* option = nullptr) const;

			inline void UpdateConfig(Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers);
			inline void UpdateConfigCallback(ConfigCallback callback);

			struct Config
			{
				std::unordered_map<nzsl::Ast::OptionHash, nzsl::Ast::ConstantSingleValue> optionValues;
			};

			struct ConfigEqual
			{
				inline bool operator()(const Config& lhs, const Config& rhs) const;
			};

			struct ConfigHasher
			{
				inline std::size_t operator()(const Config& config) const;
			};

			struct Option
			{
				nzsl::Ast::OptionHash hash;
			};

			NazaraSignal(OnShaderUpdated, UberShader* /*uberShader*/);

		private:
			nzsl::Ast::ModulePtr Validate(const nzsl::Ast::Module& module, std::unordered_map<std::string, Option, StringHash<>, std::equal_to<>>* options);

			NazaraSlot(nzsl::ModuleResolver, OnModuleUpdated, m_onShaderModuleUpdated);

			std::unordered_map<Config, std::shared_ptr<ShaderModule>, ConfigHasher, ConfigEqual> m_combinations;
			std::unordered_map<std::string, Option, StringHash<>, std::equal_to<>> m_optionIndexByName;
			std::unordered_set<std::string, StringHash<>, std::equal_to<>> m_usedModules;
			nzsl::Ast::ModulePtr m_shaderModule;
			ConfigCallback m_configCallback;
			nzsl::ShaderStageTypeFlags m_shaderStages;
	};
}

#include <Nazara/Graphics/UberShader.inl>

#endif // NAZARA_GRAPHICS_UBERSHADER_HPP
