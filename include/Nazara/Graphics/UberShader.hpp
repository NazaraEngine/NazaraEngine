// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBER_SHADER_HPP
#define NAZARA_UBER_SHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <unordered_map>

namespace Nz
{
	class ShaderModule;

	class NAZARA_GRAPHICS_API UberShader
	{
		public:
			struct Config;
			struct Option;
			UberShader(ShaderStageTypeFlags shaderStages, const ShaderAst::StatementPtr& shaderAst);
			~UberShader() = default;

			inline ShaderStageTypeFlags GetSupportedStages() const;

			const std::shared_ptr<ShaderModule>& Get(const Config& config);

			inline bool HasOption(const std::string& optionName, Pointer<const Option>* option = nullptr) const;

			static constexpr std::size_t MaximumOptionValue = 32;

			struct Config
			{
				std::array<ShaderAst::ConstantValue, MaximumOptionValue> optionValues;
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
				std::size_t index;
				ShaderAst::ExpressionType type;
			};

		private:
			std::unordered_map<Config, std::shared_ptr<ShaderModule>, ConfigHasher, ConfigEqual> m_combinations;
			std::unordered_map<std::string, Option> m_optionIndexByName;
			ShaderAst::StatementPtr m_shaderAst;
			ShaderStageTypeFlags m_shaderStages;
	};
}

#include <Nazara/Graphics/UberShader.inl>

#endif // NAZARA_UBER_SHADER_HPP
