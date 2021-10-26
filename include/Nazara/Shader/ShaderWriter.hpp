// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERWRITER_HPP
#define NAZARA_SHADER_SHADERWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/ConstantValue.hpp>
#include <string>
#include <unordered_map>

namespace Nz
{
	class NAZARA_SHADER_API ShaderWriter
	{
		public:
			struct States;

			ShaderWriter() = default;
			ShaderWriter(const ShaderWriter&) = default;
			ShaderWriter(ShaderWriter&&) = default;
			virtual ~ShaderWriter();

			struct States
			{
				std::unordered_map<std::size_t, ShaderAst::ConstantValue> optionValues;
				bool optimize = false;
				bool sanitized = false;
			};
	};
}

#endif // NAZARA_SHADER_SHADERWRITER_HPP
