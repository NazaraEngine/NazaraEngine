// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#if !defined(NAZARA_SHADERLANG_ERROR) && (!defined(NAZARA_SHADERLANG_LEXER_ERROR) || !defined(NAZARA_SHADERLANG_PARSER_ERROR) || !defined(NAZARA_SHADERLANG_COMPILER_ERROR) || !defined(NAZARA_SHADERLANG_AST_ERROR))
#error You must define NAZARA_SHADERLANG_ERROR or NAZARA_SHADERLANG_LEXER_ERROR/NAZARA_SHADERLANG_PARSER_ERROR/NAZARA_SHADERLANG_COMPILER_ERROR before including this file
#endif

#ifndef NAZARA_SHADERLANG_AST_ERROR
#define NAZARA_SHADERLANG_AST_ERROR(...) NAZARA_SHADERLANG_ERROR(A, ...)
#endif

#ifndef NAZARA_SHADERLANG_COMPILER_ERROR
#define NAZARA_SHADERLANG_COMPILER_ERROR(...) NAZARA_SHADERLANG_ERROR(C, ...)
#endif

#ifndef NAZARA_SHADERLANG_LEXER_ERROR
#define NAZARA_SHADERLANG_LEXER_ERROR(...) NAZARA_SHADERLANG_ERROR(L, ...)
#endif

#ifndef NAZARA_SHADERLANG_PARSER_ERROR
#define NAZARA_SHADERLANG_PARSER_ERROR(...) NAZARA_SHADERLANG_ERROR(P, ...)
#endif

// Lexer errors
NAZARA_SHADERLANG_LEXER_ERROR(1, BadNumber, "bad number")
NAZARA_SHADERLANG_LEXER_ERROR(2, NumberOutOfRange, "number is out of range")
NAZARA_SHADERLANG_LEXER_ERROR(3, UnfinishedString, "unfinished string")
NAZARA_SHADERLANG_LEXER_ERROR(4, UnrecognizedChar, "unrecognized character")
NAZARA_SHADERLANG_LEXER_ERROR(5, UnrecognizedToken, "unrecognized token")

// Parser errors
NAZARA_SHADERLANG_PARSER_ERROR( 1, AttributeExpectString, "attribute {} requires a string parameter", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR( 2, AttributeInvalidParameter, "invalid parameter {} for attribute {}", std::string, ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR( 3, AttributeMissingParameter, "attribute {} requires a parameter", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR( 4, AttributeMultipleUnique, "attribute {} can only be present once", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR( 5, AttributeParameterIdentifier, "attribute {} parameter can only be an identifier", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR( 6, ExpectedToken, "expected token {}, got {}", ShaderLang::TokenType, ShaderLang::TokenType)
NAZARA_SHADERLANG_PARSER_ERROR( 7, DuplicateIdentifier, "duplicate identifier")
NAZARA_SHADERLANG_PARSER_ERROR( 8, DuplicateModule, "duplicate module")
NAZARA_SHADERLANG_PARSER_ERROR( 9, InvalidVersion, "\"{}\" is not a valid version", std::string)
NAZARA_SHADERLANG_PARSER_ERROR(10, InvalidUuid, "\"{}\" is not a valid UUID", std::string)
NAZARA_SHADERLANG_PARSER_ERROR(11, MissingAttribute, "missing attribute {}", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(12, ReservedKeyword, "reserved keyword")
NAZARA_SHADERLANG_PARSER_ERROR(13, UnknownAttribute, "unknown attribute")
NAZARA_SHADERLANG_PARSER_ERROR(14, UnknownType, "unknown type")
NAZARA_SHADERLANG_PARSER_ERROR(15, UnexpectedAttribute, "unexpected attribute {}", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(16, UnexpectedEndOfFile, "unexpected end of file")
NAZARA_SHADERLANG_PARSER_ERROR(17, UnexpectedToken, "unexpected token {}", ShaderLang::TokenType)

// Compiler errors
NAZARA_SHADERLANG_COMPILER_ERROR(1, InvalidSwizzle, "invalid swizzle {}", std::string)

// AST errors
NAZARA_SHADERLANG_AST_ERROR(1, AlreadyUsedIndex, "index {} is already used", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(2, InvalidIndex, "invalid index {}", std::size_t)

#undef NAZARA_SHADERLANG_ERROR
#undef NAZARA_SHADERLANG_AST_ERROR
#undef NAZARA_SHADERLANG_COMPILER_ERROR
#undef NAZARA_SHADERLANG_LEXER_ERROR
#undef NAZARA_SHADERLANG_PARSER_ERROR
