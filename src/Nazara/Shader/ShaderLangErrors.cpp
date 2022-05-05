// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderLangErrors.hpp>
#include <fmt/format.h>
#include <string>
#include <utility>
#include <Nazara/Shader/Debug.hpp>

// https://fmt.dev/latest/api.html#udt
template <>
struct fmt::formatter<Nz::ShaderAst::AttributeType> : formatter<string_view>
{
	template <typename FormatContext>
	auto format(const Nz::ShaderAst::AttributeType& p, FormatContext& ctx) -> decltype(ctx.out())
	{
		// TODO: Add ToString
		std::string_view name = "<unhandled attribute type>";
		switch (p)
		{
			case Nz::ShaderAst::AttributeType::Binding:            name = "binding"; break;
			case Nz::ShaderAst::AttributeType::Builtin:            name = "builtin"; break;
			case Nz::ShaderAst::AttributeType::Cond:               name = "cond"; break;
			case Nz::ShaderAst::AttributeType::DepthWrite:         name = "depth_write"; break;
			case Nz::ShaderAst::AttributeType::EarlyFragmentTests: name = "early_fragment_tests"; break;
			case Nz::ShaderAst::AttributeType::Entry:              name = "entry"; break;
			case Nz::ShaderAst::AttributeType::Export:             name = "export"; break;
			case Nz::ShaderAst::AttributeType::Layout:             name = "layout"; break;
			case Nz::ShaderAst::AttributeType::Location:           name = "location"; break;
			case Nz::ShaderAst::AttributeType::LangVersion:        name = "nzsl_version"; break;
			case Nz::ShaderAst::AttributeType::Set:                name = "set"; break;
			case Nz::ShaderAst::AttributeType::Unroll:             name = "unroll"; break;
			case Nz::ShaderAst::AttributeType::Uuid:               name = "uuid"; break;
		}

		return formatter<string_view>::format(name, ctx);
	}
};

template <>
struct fmt::formatter<Nz::ShaderLang::ErrorCategory> : formatter<string_view>
{
	template <typename FormatContext>
	auto format(const Nz::ShaderLang::ErrorCategory& p, FormatContext& ctx) -> decltype(ctx.out())
	{
		return formatter<string_view>::format(ToString(p), ctx);
	}
};

template <>
struct fmt::formatter<Nz::ShaderLang::ErrorType> : formatter<string_view>
{
	template <typename FormatContext>
	auto format(const Nz::ShaderLang::ErrorType& p, FormatContext& ctx) -> decltype(ctx.out())
	{
		return formatter<string_view>::format(ToString(p), ctx);
	}
};

template <>
struct fmt::formatter<Nz::ShaderStageType> : formatter<string_view>
{
	template <typename FormatContext>
	auto format(const Nz::ShaderStageType& p, FormatContext& ctx) -> decltype(ctx.out())
	{
		// TODO: Add ToString
		std::string_view name = "<unhandled shader stage>";
		switch (p)
		{
			case Nz::ShaderStageType::Fragment: name = "fragment"; break;
			case Nz::ShaderStageType::Vertex:   name = "vertex"; break;
		}

		return formatter<string_view>::format(name, ctx);
	}
};

template <>
struct fmt::formatter<Nz::ShaderLang::TokenType> : formatter<string_view>
{
	template <typename FormatContext>
	auto format(const Nz::ShaderLang::TokenType& p, FormatContext& ctx) -> decltype(ctx.out())
	{
		return formatter<string_view>::format(ToString(p), ctx);
	}
};

namespace Nz::ShaderLang
{
	std::string_view ToString(ErrorCategory errorCategory)
	{
		switch (errorCategory)
		{
			case ErrorCategory::Ast:         return "Ast";
			case ErrorCategory::Compilation: return "Compilation";
			case ErrorCategory::Lexing:      return "Lexing";
			case ErrorCategory::Parsing:     return "Parsing";
		}

		return "<unhandled error category>";
	}

	std::string_view ToString(ErrorType errorType)
	{
		switch (errorType)
		{
#define NAZARA_SHADERLANG_ERROR(ErrorPrefix, ErrorName, ...) case ErrorType:: ErrorPrefix ## ErrorName: return #ErrorPrefix #ErrorName;

#include <Nazara/Shader/ShaderLangErrorList.hpp>
		}

		return "<unhandled error type>";
	}

	const std::string& Error::GetErrorMessage() const
	{
		if (m_errorMessage.empty())
			m_errorMessage = BuildErrorMessage();

		return m_errorMessage;
	}

	const std::string& Error::GetFullErrorMessage() const
	{
		if (m_fullErrorMessage.empty())
		{
			if (m_sourceLocation.IsValid())
			{
				std::string_view sourceFile;
				if (m_sourceLocation.file)
					sourceFile = *m_sourceLocation.file;

				if (m_sourceLocation.startLine != m_sourceLocation.endLine)
					m_fullErrorMessage = fmt::format("{}({} -> {},{} -> {}): {} error: {}", sourceFile, m_sourceLocation.startLine, m_sourceLocation.endLine, m_sourceLocation.startColumn, m_sourceLocation.endColumn, m_errorType, GetErrorMessage());
				else if (m_sourceLocation.startColumn != m_sourceLocation.endColumn)
					m_fullErrorMessage = fmt::format("{}({},{} -> {}): {} error: {}", sourceFile, m_sourceLocation.startLine, m_sourceLocation.startColumn, m_sourceLocation.endColumn, m_errorType, GetErrorMessage());
				else
					m_fullErrorMessage = fmt::format("{}({}, {}): {} error: {}", sourceFile, m_sourceLocation.startLine, m_sourceLocation.startColumn, m_errorType, GetErrorMessage());
			}
			else
				m_fullErrorMessage = fmt::format("?: {} error: {}", m_errorType, GetErrorMessage());
		}

		return m_fullErrorMessage;
	}

	const char* Error::what() const noexcept
	{
		return GetFullErrorMessage().c_str();
	}

#define NAZARA_SHADERLANG_NEWERRORTYPE(Prefix, ErrorType, ErrorName, ErrorString, ...) \
	std::string Prefix ## ErrorName ## Error::BuildErrorMessage() const \
	{ \
		return std::apply([&](const auto... args) { return fmt::format(ErrorString, args...); }, m_parameters); \
	}

#define NAZARA_SHADERLANG_AST_ERROR(ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Ast, A, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_LEXER_ERROR(ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Lexer, L, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_PARSER_ERROR(ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Parser, P, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_COMPILER_ERROR(ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Compiler, C, ErrorName, ErrorString, __VA_ARGS__)

#include <Nazara/Shader/ShaderLangErrorList.hpp>

#undef NAZARA_SHADERLANG_NEWERRORTYPE
}
