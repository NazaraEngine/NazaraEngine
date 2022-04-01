// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#if !defined(NAZARA_SHADERLANG_ERROR) && (!defined(NAZARA_SHADERLANG_LEXER_ERROR) || !defined(NAZARA_SHADERLANG_PARSER_ERROR) || !defined(NAZARA_SHADERLANG_COMPILER_ERROR) || !defined(NAZARA_SHADERLANG_AST_ERROR))
#error You must define NAZARA_SHADERLANG_ERROR or NAZARA_SHADERLANG_LEXER_ERROR/NAZARA_SHADERLANG_PARSER_ERROR/NAZARA_SHADERLANG_COMPILER_ERROR before including this file
#endif

#ifndef NAZARA_SHADERLANG_AST_ERROR
#define NAZARA_SHADERLANG_AST_ERROR(...) NAZARA_SHADERLANG_ERROR(A, __VA_ARGS__)
#endif

#ifndef NAZARA_SHADERLANG_COMPILER_ERROR
#define NAZARA_SHADERLANG_COMPILER_ERROR(...) NAZARA_SHADERLANG_ERROR(C, __VA_ARGS__)
#endif

#ifndef NAZARA_SHADERLANG_LEXER_ERROR
#define NAZARA_SHADERLANG_LEXER_ERROR(...) NAZARA_SHADERLANG_ERROR(L, __VA_ARGS__)
#endif

#ifndef NAZARA_SHADERLANG_PARSER_ERROR
#define NAZARA_SHADERLANG_PARSER_ERROR(...) NAZARA_SHADERLANG_ERROR(P, __VA_ARGS__)
#endif

// Lexer errors
NAZARA_SHADERLANG_LEXER_ERROR(BadNumber, "bad number")
NAZARA_SHADERLANG_LEXER_ERROR(NumberOutOfRange, "number is out of range")
NAZARA_SHADERLANG_LEXER_ERROR(UnfinishedString, "unfinished string")
NAZARA_SHADERLANG_LEXER_ERROR(UnrecognizedChar, "unrecognized character")
NAZARA_SHADERLANG_LEXER_ERROR(UnrecognizedToken, "unrecognized token")

// Parser errors
NAZARA_SHADERLANG_PARSER_ERROR(AttributeExpectString, "attribute {} requires a string parameter", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(AttributeInvalidParameter, "invalid parameter {} for attribute {}", std::string, ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(AttributeMissingParameter, "attribute {} requires a parameter", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(AttributeMultipleUnique, "attribute {} can only be present once", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(AttributeParameterIdentifier, "attribute {} parameter can only be an identifier", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(ExpectedToken, "expected token {}, got {}", ShaderLang::TokenType, ShaderLang::TokenType)
NAZARA_SHADERLANG_PARSER_ERROR(DuplicateIdentifier, "duplicate identifier")
NAZARA_SHADERLANG_PARSER_ERROR(DuplicateModule, "duplicate module")
NAZARA_SHADERLANG_PARSER_ERROR(InvalidVersion, "\"{}\" is not a valid version", std::string)
NAZARA_SHADERLANG_PARSER_ERROR(InvalidUuid, "\"{}\" is not a valid UUID", std::string)
NAZARA_SHADERLANG_PARSER_ERROR(MissingAttribute, "missing attribute {}", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(ReservedKeyword, "reserved keyword")
NAZARA_SHADERLANG_PARSER_ERROR(UnknownAttribute, "unknown attribute")
NAZARA_SHADERLANG_PARSER_ERROR(UnknownType, "unknown type")
NAZARA_SHADERLANG_PARSER_ERROR(UnexpectedAttribute, "unexpected attribute {}", ShaderAst::AttributeType)
NAZARA_SHADERLANG_PARSER_ERROR(UnexpectedEndOfFile, "unexpected end of file")
NAZARA_SHADERLANG_PARSER_ERROR(UnexpectedToken, "unexpected token {}", ShaderLang::TokenType)

// Compiler errors
NAZARA_SHADERLANG_COMPILER_ERROR(AliasUnexpectedType, "for now, only aliases, functions and structs can be aliased (got {})", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ArrayLength, "array length must a strictly positive integer, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(AssignTemporary, "temporary values cannot be assigned")
NAZARA_SHADERLANG_COMPILER_ERROR(AttributeUnexpectedExpression, "unexpected expression for this type")
NAZARA_SHADERLANG_COMPILER_ERROR(AttributeUnexpectedType, "unexpected attribute type")
NAZARA_SHADERLANG_COMPILER_ERROR(BinaryIncompatibleTypes, "incompatibles types ({} and {})", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(BinaryUnsupported, "{} type ({}) does not support this binary operation", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(BranchOutsideOfFunction, "non-const branching statements can only exist inside a function")
NAZARA_SHADERLANG_COMPILER_ERROR(CastComponentMismatch, "component count ({}) doesn't match required component count ({})", UInt32, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(CastIncompatibleTypes, "incompatibles types ({} and {})", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(CastMatrixExpectedVector, "expected vector type, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(CastMatrixVectorComponentMismatch, "vector component count ({}) doesn't match target matrix row count ({})", UInt32, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(CircularImport, "circular import detected on {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ConditionExpectedBool, "expected boolean for condition, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ConstMissingExpression, "const variables must have an expression")
NAZARA_SHADERLANG_COMPILER_ERROR(ConstantExpectedValue, "expected a value")
NAZARA_SHADERLANG_COMPILER_ERROR(ConstantExpressionRequired, "a constant expression is required in this context")
NAZARA_SHADERLANG_COMPILER_ERROR(DepthWriteAttribute, "only fragment entry-points can have the depth_write attribute")
NAZARA_SHADERLANG_COMPILER_ERROR(DiscardEarlyFragmentTests, "discard is not compatible with early fragment tests")
NAZARA_SHADERLANG_COMPILER_ERROR(DiscardOutsideOfFragmentStage, "discard can only be used in the fragment stage (function gets called in the {} stage)", ShaderStageType)
NAZARA_SHADERLANG_COMPILER_ERROR(DiscardOutsideOfFunction, "discard can only be used inside a function")
NAZARA_SHADERLANG_COMPILER_ERROR(EarlyFragmentTestsAttribute, "only functions with entry(frag) attribute can have the early_fragments_tests attribute")
NAZARA_SHADERLANG_COMPILER_ERROR(EntryFunctionParameter, "entry functions can either take one struct parameter or no parameter")
NAZARA_SHADERLANG_COMPILER_ERROR(EntryPointAlreadyDefined, "the {} entry type has been defined multiple times", ShaderStageType)
NAZARA_SHADERLANG_COMPILER_ERROR(ExpectedFunction, "expected function expression")
NAZARA_SHADERLANG_COMPILER_ERROR(ExpectedIntrinsicFunction, "expected intrinsic function expression")
NAZARA_SHADERLANG_COMPILER_ERROR(ExpectedPartialType, "only partial types can be specialized, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ExtAlreadyDeclared, "external variable {} is already declared", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ExtBindingAlreadyUsed, "binding (set={}, binding={}) is already in use", UInt32, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(ExtMissingBindingIndex, "external variable requires a binding index")
NAZARA_SHADERLANG_COMPILER_ERROR(ExtTypeNotAllowed, "external variable {} is of wrong type ({}): only uniform and sampler are allowed in external blocks", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ForEachUnsupportedType, "for-each statements can only be called on array types, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ForFromTypeExpectIntegerType, "numerical for from expression must be an integer or unsigned integer, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ForStepUnmatchingType, "numerical for step expression type ({}) must match from expression type ({})", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ForToUnmatchingType, "numerical for to expression type ({}) must match from expression type ({})", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(FullTypeExpected, "expected a full type, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(FunctionCallExpectedFunction, "expected function expression")
NAZARA_SHADERLANG_COMPILER_ERROR(FunctionCallOutsideOfFunction, "function calls must happen inside a function")
NAZARA_SHADERLANG_COMPILER_ERROR(FunctionCallUnexpectedEntryFunction, "{} is an entry function which cannot be called by the program", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(FunctionCallUnmatchingParameterCount, "function {} expects {} parameter(s), but got {}", std::string, UInt32, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(FunctionCallUnmatchingParameterType, "function {} parameter #{} type mismatch (expected {}, got {})", std::string, UInt32, std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(FunctionDeclarationInsideFunction, "a function cannot be defined inside another function")
NAZARA_SHADERLANG_COMPILER_ERROR(IdentifierAlreadyUsed, "identifier {} is already used", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(IndexRequiresIntegerIndices, "index access requires integer indices (got {})", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(IndexStructRequiresInt32Indices, "struct indexing requires constant i32 indices (got {})", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(IndexUnexpectedType, "unexpected type: only arrays, structs, vectors and matrices can be indexed (got {})", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(IntrinsicExpectedFloat, "expected scalar or vector floating-points")
NAZARA_SHADERLANG_COMPILER_ERROR(IntrinsicExpectedParameterCount, "expected {} parameter(s)", UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(IntrinsicExpectedType, "expected type {1} for parameter #{0}, got {2}", UInt32, std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(IntrinsicUnexpectedBoolean, "boolean parameters are not allowed")
NAZARA_SHADERLANG_COMPILER_ERROR(IntrinsicUnmatchingParameterType, "all types must match")
NAZARA_SHADERLANG_COMPILER_ERROR(InvalidScalarSwizzle, "invalid swizzle for scalar")
NAZARA_SHADERLANG_COMPILER_ERROR(InvalidSwizzle, "invalid swizzle {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(MissingOptionValue, "option {} requires a value (no default value set)", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ModuleCompilationFailed, "module {} compilation failed: {}", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(ModuleNotFound, "module {} not found", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(NoModuleResolver, "import statement found but no module resolver has been set (and partial sanitization is not enabled)")
NAZARA_SHADERLANG_COMPILER_ERROR(OptionDeclarationInsideFunction, "options must be declared outside of functions")
NAZARA_SHADERLANG_COMPILER_ERROR(PartialTypeExpect, "expected a {} type at #{}", std::string, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(PartialTypeParameterCountMismatch, "parameter count mismatch (expected {}, got {})", UInt32, UInt32)
NAZARA_SHADERLANG_COMPILER_ERROR(SamplerUnexpectedType, "for now only f32 samplers are supported (got {})", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(StructDeclarationInsideFunction, "structs must be declared outside of functions")
NAZARA_SHADERLANG_COMPILER_ERROR(StructExpected, "struct type expected, got {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(StructFieldBuiltinLocation, "a struct field cannot have both builtin and location attributes")
NAZARA_SHADERLANG_COMPILER_ERROR(StructFieldMultiple, "multiple {} active struct field found, only one can be active at a time", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(StructLayoutInnerMismatch, "inner struct layout mismatch, struct is declared with {} but field has layout {}", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(StructLayoutTypeNotAllowed, "{} type is not allowed in {} layout", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(SwizzleUnexpectedType, "expression type ({}) does not support swizzling", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(UnaryUnsupported, "type ({}) does not support this unary operation", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(UnexpectedAccessedType, "unexpected type (only struct and vectors can be indexed with identifiers)")
NAZARA_SHADERLANG_COMPILER_ERROR(UnknownField, "unknown field {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(UnknownIdentifier, "unknown identifier {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(UnknownMethod, "unknown method {}", std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(UnmatchingTypes, "left expression type ({}) doesn't match right expression type ({})", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(VarDeclarationMissingTypeAndValue, "variable must either have a type or an initial value")
NAZARA_SHADERLANG_COMPILER_ERROR(VarDeclarationOutsideOfFunction, "global variables outside of external blocks are forbidden")
NAZARA_SHADERLANG_COMPILER_ERROR(VarDeclarationTypeUnmatching, "initial expression type ({}) doesn't match specified type ({})", std::string, std::string)
NAZARA_SHADERLANG_COMPILER_ERROR(WhileUnrollNotSupported, "unroll(always) is not yet supported on while, use a for loop")

// AST errors
NAZARA_SHADERLANG_AST_ERROR(AlreadyUsedIndex, "index {} is already used", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(AttributeRequiresValue, "index {} is already used", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(AlreadyUsedIndexPreregister, "cannot preregister used index {} as its already used", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(EmptyIdentifier, "identifier cannot be empty")
NAZARA_SHADERLANG_AST_ERROR(Internal, "internal error: {}", std::string)
NAZARA_SHADERLANG_AST_ERROR(InvalidConstantIndex, "invalid constant index #{}", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(InvalidIndex, "invalid index {}", std::size_t)
NAZARA_SHADERLANG_AST_ERROR(MissingExpression, "a mandatory expression is missing")
NAZARA_SHADERLANG_AST_ERROR(MissingStatement, "a mandatory statement is missing")
NAZARA_SHADERLANG_AST_ERROR(NoIdentifier, "at least one identifier is required")
NAZARA_SHADERLANG_AST_ERROR(NoIndex, "at least one index is required")
NAZARA_SHADERLANG_AST_ERROR(UnexpectedIdentifier, "unexpected identifier of type {}", std::string)

#undef NAZARA_SHADERLANG_ERROR
#undef NAZARA_SHADERLANG_AST_ERROR
#undef NAZARA_SHADERLANG_COMPILER_ERROR
#undef NAZARA_SHADERLANG_LEXER_ERROR
#undef NAZARA_SHADERLANG_PARSER_ERROR
