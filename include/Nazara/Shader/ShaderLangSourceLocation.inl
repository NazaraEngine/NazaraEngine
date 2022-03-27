// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangSourceLocation.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderLang
{
	inline SourceLocation::SourceLocation() :
	endColumn(0),
	endLine(0),
	startColumn(0),
	startLine(0)
	{
	}

	inline SourceLocation::SourceLocation(unsigned int Line, unsigned int Column, std::shared_ptr<const std::string> File) :
	file(std::move(File)),
	endColumn(Column),
	endLine(Line),
	startColumn(Column),
	startLine(Line)
	{
	}

	inline SourceLocation::SourceLocation(unsigned int Line, unsigned int StartColumn, unsigned int EndColumn, std::shared_ptr<const std::string> File) :
	file(std::move(File)),
	endColumn(EndColumn),
	endLine(Line),
	startColumn(StartColumn),
	startLine(Line)
	{
	}

	inline SourceLocation::SourceLocation(unsigned int StartLine, unsigned int EndLine, unsigned int StartColumn, unsigned int EndColumn, std::shared_ptr<const std::string> File) :
	file(std::move(File)),
	endColumn(EndColumn),
	endLine(EndLine),
	startColumn(StartColumn),
	startLine(StartLine)
	{
	}

	inline bool SourceLocation::IsValid() const
	{
		return startLine != 0 && endLine != 0 && endColumn != 0 && startColumn != 0;
	}
}

#include <Nazara/Shader/DebugOff.hpp>
