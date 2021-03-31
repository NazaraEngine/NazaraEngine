// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UBER_SHADER_HPP
#define NAZARA_UBER_SHADER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Shader/ShaderNodes.hpp>
#include <unordered_map>

namespace Nz
{
	class ShaderModule;

	class NAZARA_GRAPHICS_API UberShader
	{
		public:
			UberShader(ShaderStageType shaderStage, ShaderAst::StatementPtr shaderAst);
			~UberShader() = default;

			UInt64 GetConditionFlagByName(const std::string_view& condition) const;

			const std::shared_ptr<ShaderModule>& Get(UInt64 combination);

		private:
			std::unordered_map<UInt64 /*combination*/, std::shared_ptr<ShaderModule>> m_combinations;
			ShaderAst::StatementPtr m_shaderAst;
			ShaderStageType m_shaderStage;
			UInt64 m_combinationMask;
	};
}

#include <Nazara/Graphics/UberShader.inl>

#endif // NAZARA_UBER_SHADER_HPP
