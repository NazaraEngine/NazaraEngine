// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_UBERSHADER_HPP
#define NAZARA_GRAPHICS_UBERSHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Shader/ShaderModuleResolver.hpp>
#include <Nazara/Shader/Ast/Module.hpp>
#include <unordered_map>

namespace Nz
{
	class ShaderModule;

	class NAZARA_GRAPHICS_API UberShader
	{
		public:
			struct Config;
			struct Option;
			using ConfigCallback = std::function<void(Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers)>;

			UberShader(ShaderStageTypeFlags shaderStages, std::string moduleName);
			UberShader(ShaderStageTypeFlags shaderStages, ShaderModuleResolver& moduleResolver, std::string moduleName);
			UberShader(ShaderStageTypeFlags shaderStages, ShaderAst::ModulePtr shaderModule);
			~UberShader() = default;

			inline ShaderStageTypeFlags GetSupportedStages() const;

			const std::shared_ptr<ShaderModule>& Get(const Config& config);

			inline bool HasOption(const std::string& optionName, Pointer<const Option>* option = nullptr) const;

			inline void UpdateConfig(Config& config, const std::vector<RenderPipelineInfo::VertexBufferData>& vertexBuffers);
			inline void UpdateConfigCallback(ConfigCallback callback);

			struct Config
			{
				std::unordered_map<UInt32, ShaderAst::ConstantValue> optionValues;
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
				UInt32 hash;
			};

			NazaraSignal(OnShaderUpdated, UberShader* /*uberShader*/);

		private:
			void Validate(ShaderAst::Module& module);

			NazaraSlot(ShaderModuleResolver, OnModuleUpdated, m_onShaderModuleUpdated);

			std::unordered_map<Config, std::shared_ptr<ShaderModule>, ConfigHasher, ConfigEqual> m_combinations;
			std::unordered_map<std::string, Option> m_optionIndexByName;
			ShaderAst::ModulePtr m_shaderModule;
			ConfigCallback m_configCallback;
			ShaderStageTypeFlags m_shaderStages;
	};
}

#include <Nazara/Graphics/UberShader.inl>

#endif // NAZARA_GRAPHICS_UBERSHADER_HPP
