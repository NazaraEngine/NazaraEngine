// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERWRITER_HPP
#define NAZARA_SHADERWRITER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <string>

namespace Nz
{
	class ShaderAst;

	class NAZARA_RENDERER_API ShaderWriter
	{
		public:
			ShaderWriter() = default;
			ShaderWriter(const ShaderWriter&) = delete;
			ShaderWriter(ShaderWriter&&) = delete;
			virtual ~ShaderWriter();

			virtual std::string Generate(const ShaderAst& shader) = 0;
	};
}

#endif // NAZARA_SHADERWRITER_HPP
