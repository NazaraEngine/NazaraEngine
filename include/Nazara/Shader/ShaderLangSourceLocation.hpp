// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERLANGSOURCELOCATION_HPP
#define NAZARA_SHADER_SHADERLANGSOURCELOCATION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <memory>
#include <string>

namespace Nz::ShaderLang
{
	struct SourceLocation
	{
		inline SourceLocation();
		inline SourceLocation(unsigned int line, unsigned int column, std::shared_ptr<const std::string> file);
		inline SourceLocation(unsigned int line, unsigned int startColumn, unsigned int endColumn, std::shared_ptr<const std::string> file);
		inline SourceLocation(unsigned int startLine, unsigned int endLine, unsigned int startColumn, unsigned int endColumn, std::shared_ptr<const std::string> file);

		inline bool IsValid() const;

		std::shared_ptr<const std::string> file; //< Since the same file will be used for every node, prevent holding X time the same path
		UInt32 endColumn;
		UInt32 endLine;
		UInt32 startColumn;
		UInt32 startLine;
	};
}

#include <Nazara/Shader/ShaderLangSourceLocation.inl>

#endif // NAZARA_SHADER_SHADERLANGSOURCELOCATION_HPP
