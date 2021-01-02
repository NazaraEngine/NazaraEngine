// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERWRITER_HPP
#define NAZARA_SHADERWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <string>
#include <unordered_set>

namespace Nz
{
	class ShaderAst;

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
				Nz::UInt64 enabledConditions = 0;
			};
	};
}

#endif // NAZARA_SHADERWRITER_HPP
