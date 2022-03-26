// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERLANGERRORS_HPP
#define NAZARA_SHADER_SHADERLANGERRORS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/Enums.hpp>
#include <exception>
#include <memory>
#include <string>
#include <tuple>

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
		unsigned int endColumn;
		unsigned int endLine;
		unsigned int startColumn;
		unsigned int startLine;
	};

	enum class ErrorCategory
	{
		Ast,
		Compilation,
		Lexing,
		Parsing,

		Max = Parsing
	};

	class Error : public std::exception
	{
		public:
			inline Error(SourceLocation sourceLocation, ErrorCategory errorCategory, unsigned int errorType) noexcept;
			Error(const Error&) = delete;
			Error(Error&&) = delete;
			~Error() = default;

			inline ErrorCategory GetErrorCategory() const;
			const std::string& GetErrorMessage() const;
			inline unsigned int GetErrorType() const;
			inline const SourceLocation& GetSourceLocation() const;

			const char* what() const noexcept override;

			Error& operator=(const Error&) = delete;
			Error& operator=(Error&&) = delete;

		protected:
			virtual std::string BuildErrorMessage() const = 0;

		private:
			mutable std::string m_errorMessage;
			ErrorCategory m_errorCategory;
			SourceLocation m_sourceLocation;
			unsigned int m_errorType;
	};

	class AstError : public Error
	{
		public:
			inline AstError(SourceLocation sourceLocation, unsigned int errorType) noexcept;
	};

	class CompilationError : public Error
	{
		public:
			inline CompilationError(SourceLocation sourceLocation, unsigned int errorType) noexcept;
	};

	class LexingError : public Error
	{
		public:
			inline LexingError(SourceLocation sourceLocation, unsigned int errorType) noexcept;
	};
	
	class ParsingError : public Error
	{
		public:
			inline ParsingError(SourceLocation sourceLocation, unsigned int errorType) noexcept;
	};

#define NAZARA_SHADERLANG_NEWERRORTYPE(Prefix, BaseClass, ErrorType, ErrorName, ErrorString, ...) \
	class Prefix ## ErrorName ## Error : public BaseClass \
	{ \
		public: \
			template<typename... Args> Prefix ## ErrorName ## Error(SourceLocation sourceLocation, Args&&... args) : \
			BaseClass(std::move(sourceLocation), ErrorType), \
			m_parameters(std::forward<Args>(args)...) \
			{ \
			} \
		\
		private: \
			std::string BuildErrorMessage() const override; \
			\
			std::tuple<__VA_ARGS__> m_parameters; \
	};

#define NAZARA_SHADERLANG_AST_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Ast, AstError, ErrorType, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_LEXER_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Lexer, LexingError, ErrorType, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_PARSER_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Parser, ParsingError, ErrorType, ErrorName, ErrorString, __VA_ARGS__)
#define NAZARA_SHADERLANG_COMPILER_ERROR(ErrorType, ErrorName, ErrorString, ...) NAZARA_SHADERLANG_NEWERRORTYPE(Compiler, CompilationError, ErrorType, ErrorName, ErrorString, __VA_ARGS__)

#include <Nazara/Shader/ShaderLangErrorList.hpp>

#undef NAZARA_SHADERLANG_NEWERRORTYPE
}

#include <Nazara/Shader/ShaderLangErrors.inl>

#endif // NAZARA_SHADER_SHADERLANGERRORS_HPP
