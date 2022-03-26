// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangErrors.hpp>
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

	inline Error::Error(SourceLocation sourceLocation, ErrorCategory errorCategory, unsigned int errorType) noexcept :
	m_errorCategory(errorCategory),
	m_sourceLocation(std::move(sourceLocation)),
	m_errorType(errorType)
	{
	}

	inline ErrorCategory Error::GetErrorCategory() const
	{
		return m_errorCategory;
	}

	inline unsigned int Error::GetErrorType() const
	{
		return m_errorType;
	}

	inline const SourceLocation& Error::GetSourceLocation() const
	{
		return m_sourceLocation;
	}


	inline AstError::AstError(SourceLocation sourceLocation, unsigned int errorType) noexcept :
	Error(std::move(sourceLocation), ErrorCategory::Ast, errorType)
	{
	}

	inline CompilationError::CompilationError(SourceLocation sourceLocation, unsigned int errorType) noexcept :
	Error(std::move(sourceLocation), ErrorCategory::Compilation, errorType)
	{
	}

	inline ParsingError::ParsingError(SourceLocation sourceLocation, unsigned int errorType) noexcept :
	Error(std::move(sourceLocation), ErrorCategory::Parsing, errorType)
	{
	}

	inline LexingError::LexingError(SourceLocation sourceLocation, unsigned int errorType) noexcept :
	Error(std::move(sourceLocation), ErrorCategory::Lexing, errorType)
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
