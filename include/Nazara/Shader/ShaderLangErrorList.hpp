// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#if !defined(NAZARA_SHADERLANG_ERROR) && (!defined(NAZARA_SHADERLANG_LEXER_ERROR) || !defined(NAZARA_SHADERLANG_PARSER_ERROR) || !defined(NAZARA_SHADERLANG_COMPILER_ERROR))
#error You must define NAZARA_SHADERLANG_ERROR or NAZARA_SHADERLANG_LEXER_ERROR/NAZARA_SHADERLANG_PARSER_ERROR/NAZARA_SHADERLANG_COMPILER_ERROR before including this file
#endif

#ifndef NAZARA_SHADERLANG_LEXER_ERROR
#define NAZARA_SHADERLANG_LEXER_ERROR(...) NAZARA_SHADERLANG_ERROR(L, ...)
#endif

#ifndef NAZARA_SHADERLANG_PARSER_ERROR
#define NAZARA_SHADERLANG_PARSER_ERROR(...) NAZARA_SHADERLANG_ERROR(P, ...)
#endif

#ifndef NAZARA_SHADERLANG_COMPILER_ERROR
#define NAZARA_SHADERLANG_COMPILER_ERROR(...) NAZARA_SHADERLANG_COMPILER_ERROR(C, ...)
#endif

// Lexer errors
NAZARA_SHADERLANG_LEXER_ERROR(1, BadNumber, "bad number")
NAZARA_SHADERLANG_LEXER_ERROR(2, NumberOutOfRange, "number is out of range")
NAZARA_SHADERLANG_LEXER_ERROR(3, UnfinishedString, "unfinished string")
NAZARA_SHADERLANG_LEXER_ERROR(4, UnrecognizedChar, "unrecognized character")
NAZARA_SHADERLANG_LEXER_ERROR(5, UnrecognizedToken, "unrecognized token")

// Parser errors
NAZARA_SHADERLANG_PARSER_ERROR(1, AttributeError, "attribute error")
NAZARA_SHADERLANG_PARSER_ERROR(2, ExpectedToken, "expected token")
NAZARA_SHADERLANG_PARSER_ERROR(3, DuplicateIdentifier, "duplicate identifier")
NAZARA_SHADERLANG_PARSER_ERROR(4, DuplicateModule, "duplicate module")
NAZARA_SHADERLANG_PARSER_ERROR(5, ReservedKeyword, "reserved keyword")
NAZARA_SHADERLANG_PARSER_ERROR(6, UnknownAttribute, "unknown attribute")
NAZARA_SHADERLANG_PARSER_ERROR(7, UnknownType, "unknown type")
NAZARA_SHADERLANG_PARSER_ERROR(8, UnexpectedToken, "unexpected token")

// Compiler errors

#undef NAZARA_SHADERLANG_ERROR
#undef NAZARA_SHADERLANG_COMPILER_ERROR
#undef NAZARA_SHADERLANG_LEXER_ERROR
#undef NAZARA_SHADERLANG_PARSER_ERROR
