// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBER_SHADER_HPP
#define NAZARA_UBER_SHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <unordered_map>

namespace Nz
{
	class ShaderModule;

	class NAZARA_GRAPHICS_API UberShader
	{
		public:
			UberShader(ShaderStageTypeFlags shaderStages, const ShaderAst::StatementPtr& shaderAst);
			~UberShader() = default;

			UInt64 GetOptionFlagByName(const std::string& optionName) const;

			inline ShaderStageTypeFlags GetSupportedStages() const;

			const std::shared_ptr<ShaderModule>& Get(UInt64 combination);

		private:
			std::unordered_map<UInt64 /*combination*/, std::shared_ptr<ShaderModule>> m_combinations;
			std::unordered_map<std::string, std::size_t> m_optionIndexByName;
			ShaderAst::StatementPtr m_shaderAst;
			ShaderStageTypeFlags m_shaderStages;
			UInt64 m_combinationMask;
	};
}

#include <Nazara/Graphics/UberShader.inl>

#endif // NAZARA_UBER_SHADER_HPP
