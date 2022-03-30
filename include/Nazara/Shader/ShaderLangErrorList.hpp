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
NAZARA_SHADERLANG_COMPILER_ERROR(2, BinaryIncompatibleTypes, "incompatibles types (<TODO> and <TODO>)")
NAZARA_SHADERLANG_COMPILER_ERROR(2, BinaryUnsupported, "{} type (<TODO>) does not support this binary operation", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, BranchOutsideOfFunction, "non-const branching statements can only exist inside a function")
NAZARA_SHADERLANG_COMPILER_ERROR(2, CastIncompatibleTypes, "incompatibles types (<TODO> and <TODO>)")
NAZARA_SHADERLANG_COMPILER_ERROR(2, CastComponentMismatch, "component count doesn't match required component count")
NAZARA_SHADERLANG_COMPILER_ERROR(2, CircularImport, "circular import detected on {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, ConditionExpectedBool, "expected a boolean value")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ConstMissingExpression, "const variables must have an expression")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ConstantExpectedValue, "expected a value")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ConstantExpressionRequired, "a constant expression is required in this context")
NAZARA_SHADERLANG_COMPILER_ERROR(2, DepthWriteAttribute, "only fragment entry-points can have the depth_write attribute")
NAZARA_SHADERLANG_COMPILER_ERROR(2, DiscardEarlyFragmentTests, "discard is not compatible with early fragment tests")
NAZARA_SHADERLANG_COMPILER_ERROR(2, EarlyFragmentTestsAttribute, "only functions with entry(frag) attribute can have the early_fragments_tests attribute")
NAZARA_SHADERLANG_COMPILER_ERROR(2, EntryFunctionParameter, "entry functions can either take one struct parameter or no parameter")
NAZARA_SHADERLANG_COMPILER_ERROR(2, EntryPointAlreadyDefined, "the same entry type has been defined multiple times")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ExpectedFunction, "expected function expression")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ExpectedIntrinsicFunction, "expected intrinsic function expression")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ExtAlreadyDeclared, "external variable {} is already declared", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, ExtTypeNotAllowed, "external variable {} is of wrong type: only uniform and sampler are allowed in external blocks", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, ExtBindingAlreadyUsed, "binding (set={}, binding={}) is already in use", UInt32, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(2, ExtMissingBindingIndex, "external variable requires a binding index")
NAZARA_SHADERLANG_COMPILER_ERROR(2, ForEachUnsupportedType, "for-each statements can only be called on array types, got <TODO>")
NAZARA_SHADERLANG_COMPILER_ERROR(2, FunctionCallOutsideOfFunction, "function calls must happen inside a function")
NAZARA_SHADERLANG_COMPILER_ERROR(2, FunctionDeclarationInsideFunction, "a function cannot be defined inside another function")
NAZARA_SHADERLANG_COMPILER_ERROR(2, IdentifierAlreadyUsed, "identifier {} is already used", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, IntrinsicExpectedParameterCount, "expected {} parameter(s)", unsigned int)
NAZARA_SHADERLANG_COMPILER_ERROR(2, IntrinsicExpectedFloat, "expected scalar or vector floating-points")
NAZARA_SHADERLANG_COMPILER_ERROR(2, IntrinsicExpectedType, "expected type <TODO> for parameter #{}, got <TODO>", unsigned int)
NAZARA_SHADERLANG_COMPILER_ERROR(2, IntrinsicUnexpectedBoolean, "boolean parameters are not allowed")
NAZARA_SHADERLANG_COMPILER_ERROR(2, IntrinsicUnmatchingParameterType, "all types must match")
NAZARA_SHADERLANG_COMPILER_ERROR(2, InvalidScalarSwizzle, "invalid swizzle for scalar")
NAZARA_SHADERLANG_COMPILER_ERROR(2, InvalidSwizzle, "invalid swizzle {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, MissingOptionValue, "option {} requires a value (no default value set)", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, PartialTypeExpect, "expected a {} type at #{}", std::string, unsigned int)
NAZARA_SHADERLANG_COMPILER_ERROR(2, StructDeclarationInsideFunction, "structs must be declared outside of functions")
NAZARA_SHADERLANG_COMPILER_ERROR(2, VarDeclarationMissingTypeAndValue, "variable must either have a type or an initial value")
NAZARA_SHADERLANG_COMPILER_ERROR(2, VarDeclarationTypeUnmatching, "initial expression type (<TODO>) doesn't match specified type (<TODO>)")
NAZARA_SHADERLANG_COMPILER_ERROR(2, UnexpectedAccessedType, "unexpected type (only struct and vectors can be indexed with identifiers)")
NAZARA_SHADERLANG_COMPILER_ERROR(2, UnaryUnsupported, "type (<TODO>) does not support this unary operation", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, UnmatchingTypes, "left expression type (<TODO>) doesn't match right expression type (<TODO>)")
NAZARA_SHADERLANG_COMPILER_ERROR(2, UnknownField, "unknown field {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, UnknownMethod, "unknown method {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, UnknownIdentifier, "unknown identifier {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(2, WhileUnrollNotSupported, "unroll(always) is not yet supported on while")

// AST errors
NAZARA_SHADERLANG_AST_ERROR(1, AlreadyUsedIndex, "index {} is already used", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(2, AlreadyUsedIndexPreregister, "cannot preregister used index {} as its already used", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(2, EmptyIdentifier, "identifier cannot be empty")
NAZARA_SHADERLANG_AST_ERROR(2, Internal, "internal error: {}", std::string)
NAZARA_SHADERLANG_AST_ERROR(2, InvalidConstantIndex, "invalid constant index #{}", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(2, InvalidIndex, "invalid index {}", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(2, MissingExpression, "a mandatory expression is missing")
NAZARA_SHADERLANG_AST_ERROR(2, MissingStatement, "a mandatory statement is missing")
NAZARA_SHADERLANG_AST_ERROR(2, NoIdentifier, "at least one identifier is required")
NAZARA_SHADERLANG_AST_ERROR(2, NoIndex, "at least one index is required")
NAZARA_SHADERLANG_AST_ERROR(2, UnexpectedIdentifier, "unexpected identifier of type {}", std::string)

#undef NAZARA_SHADERLANG_ERROR
#undef NAZARA_SHADERLANG_AST_ERROR
#undef NAZARA_SHADERLANG_COMPILER_ERROR
#undef NAZARA_SHADERLANG_LEXER_ERROR
#undef NAZARA_SHADERLANG_PARSER_ERROR
