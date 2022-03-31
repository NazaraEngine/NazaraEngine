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
		// TODO: Add ToString
		std::string_view name = "<unhandled error category>";
		switch (p)
		{
			case Nz::ShaderLang::ErrorCategory::Ast:         name = "Ast"; break;
			case Nz::ShaderLang::ErrorCategory::Compilation: name = "Compilation"; break;
			case Nz::ShaderLang::ErrorCategory::Lexing:      name = "Lexing"; break;
			case Nz::ShaderLang::ErrorCategory::Parsing:     name = "Parsing"; break;
		}

		return formatter<string_view>::format(name, ctx);
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
	const std::string& Error::GetErrorMessage() const
	{
		if (m_errorMessage.empty())
		{
			if (m_sourceLocation.IsValid())
			{
				std::string_view sourceFile;
				if (m_sourceLocation.file)
					sourceFile = *m_sourceLocation.file;

				if (m_sourceLocation.startLine != m_sourceLocation.endLine)
					m_errorMessage = fmt::format("{}({} -> {},{} -> {}): {} error {}: {}", sourceFile, m_sourceLocation.startLine, m_sourceLocation.endLine, m_sourceLocation.startColumn, m_sourceLocation.endColumn, m_errorCategory, m_errorType, BuildErrorMessage());
				else if (m_sourceLocation.startColumn != m_sourceLocation.endColumn)
					m_errorMessage = fmt::format("{}({},{} -> {}): {} error {}: {}", sourceFile, m_sourceLocation.startLine, m_sourceLocation.startColumn, m_sourceLocation.endColumn, m_errorCategory, m_errorType, BuildErrorMessage());
				else
					m_errorMessage = fmt::format("{}({}, {}): {} error {}: {}", sourceFile, m_sourceLocation.startLine, m_sourceLocation.startColumn, m_errorCategory, m_errorType, BuildErrorMessage());
			}
			else
				m_errorMessage = fmt::format("?: {} error {}: {}", m_errorCategory, m_errorType, BuildErrorMessage());
		}

		return m_errorMessage;
	}

	const char* Error::what() const noexcept
	{
		return GetErrorMessage().c_str();
	}

#define NAZARA_SHADERLANG_NEWERRORTYPE(Prefix, ErrorType, ErrorName, ErrorString, ...) \
	std::string Prefix ## ErrorName ## Error::BuildErrorMessage() const \
	{ \
		return std::apply([&](const auto... args) { return fmt::format(ErrorString, args...); }, m_parameters); \
	}

#define NAZARA_SHADERLANG_AST_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Ast, ErrorType, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_LEXER_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Lexer, ErrorType, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_PARSER_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Parser, ErrorType, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_COMPILER_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Compiler, ErrorType, ErrorName, ErrorString, __VA_ARGS__)

#include <Nazara/Shader/ShaderLangErrorList.hpp>

#undef NAZARA_SHADERLANG_NEWERRORTYPE
}
