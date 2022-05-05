// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_SANITIZEVISITOR_HPP
#define NAZARA_SHADER_AST_SANITIZEVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderModuleResolver.hpp>
#include <Nazara/Shader/Ast/AstCloner.hpp>
#include <Nazara/Shader/Ast/AstTypes.hpp>
#include <Nazara/Shader/Ast/Module.hpp>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API SanitizeVisitor final : AstCloner
	{
		friend class AstTypeExpressionVisitor;

		public:
			struct Options;

			SanitizeVisitor() = default;
			SanitizeVisitor(const SanitizeVisitor&) = delete;
			SanitizeVisitor(SanitizeVisitor&&) = delete;
			~SanitizeVisitor() = default;

			inline ModulePtr Sanitize(const Module& module, std::string* error = nullptr);
			ModulePtr Sanitize(const Module& module, const Options& options, std::string* error = nullptr);

			SanitizeVisitor& operator=(const SanitizeVisitor&) = delete;
			SanitizeVisitor& operator=(SanitizeVisitor&&) = delete;

			struct Options
			{
				std::shared_ptr<ShaderModuleResolver> moduleResolver;
				std::unordered_set<std::string> reservedIdentifiers;
				std::unordered_map<UInt32, ConstantValue> optionValues;
				bool allowPartialSanitization = false;
				bool makeVariableNameUnique = false;
				bool reduceLoopsToWhile = false;
				bool removeAliases = false;
				bool removeConstDeclaration = false;
				bool removeCompoundAssignments = false;
				bool removeMatrixCast = false;
				bool removeOptionDeclaration = false;
				bool removeScalarSwizzling = false;
				bool splitMultipleBranches = false;
				bool useIdentifierAccessesForStructs = true;
			};

		private:
			enum class IdentifierCategory;
			enum class ValidationResult;
			struct CurrentFunctionData;
			struct Environment;
			struct FunctionData;
			struct Identifier;
			struct IdentifierData;
			template<typename T> struct IdentifierList;
			struct NamedPartialType;
			struct Scope;

			using AstCloner::CloneExpression;
			ExpressionValue<ExpressionType> CloneType(const ExpressionValue<ExpressionType>& exprType) override;

			ExpressionPtr Clone(AccessIdentifierExpression& node) override;
			ExpressionPtr Clone(AccessIndexExpression& node) override;
			ExpressionPtr Clone(AliasValueExpression& node) override;
			ExpressionPtr Clone(AssignExpression& node) override;
			ExpressionPtr Clone(BinaryExpression& node) override;
			ExpressionPtr Clone(CallFunctionExpression& node) override;
			ExpressionPtr Clone(CastExpression& node) override;
			ExpressionPtr Clone(ConditionalExpression& node) override;
			ExpressionPtr Clone(ConstantValueExpression& node) override;
			ExpressionPtr Clone(ConstantExpression& node) override;
			ExpressionPtr Clone(IdentifierExpression& node) override;
			ExpressionPtr Clone(IntrinsicExpression& node) override;
			ExpressionPtr Clone(SwizzleExpression& node) override;
			ExpressionPtr Clone(UnaryExpression& node) override;
			ExpressionPtr Clone(VariableValueExpression& node) override;

			StatementPtr Clone(BranchStatement& node) override;
			StatementPtr Clone(ConditionalStatement& node) override;
			StatementPtr Clone(DeclareAliasStatement& node) override;
			StatementPtr Clone(DeclareConstStatement& node) override;
			StatementPtr Clone(DeclareExternalStatement& node) override;
			StatementPtr Clone(DeclareFunctionStatement& node) override;
			StatementPtr Clone(DeclareOptionStatement& node) override;
			StatementPtr Clone(DeclareStructStatement& node) override;
			StatementPtr Clone(DeclareVariableStatement& node) override;
			StatementPtr Clone(DiscardStatement& node) override;
			StatementPtr Clone(ExpressionStatement& node) override;
			StatementPtr Clone(ForStatement& node) override;
			StatementPtr Clone(ForEachStatement& node) override;
			StatementPtr Clone(ImportStatement& node) override;
			StatementPtr Clone(MultiStatement& node) override;
			StatementPtr Clone(ScopedStatement& node) override;
			StatementPtr Clone(WhileStatement& node) override;

			const IdentifierData* FindIdentifier(const std::string_view& identifierName) const;
			template<typename F> const IdentifierData* FindIdentifier(const std::string_view& identifierName, F&& functor) const;
			const IdentifierData* FindIdentifier(const Environment& environment, const std::string_view& identifierName) const;
			template<typename F> const IdentifierData* FindIdentifier(const Environment& environment, const std::string_view& identifierName, F&& functor) const;

			const ExpressionType* GetExpressionType(Expression& expr) const;
			const ExpressionType& GetExpressionTypeSecure(Expression& expr) const;

			ExpressionPtr HandleIdentifier(const IdentifierData* identifierData, const ShaderLang::SourceLocation& sourceLocation);

			void PushScope();
			void PopScope();

			ExpressionPtr CacheResult(ExpressionPtr expression);

			std::optional<ConstantValue> ComputeConstantValue(Expression& expr) const;
			template<typename T> ValidationResult ComputeExprValue(ExpressionValue<T>& attribute, const ShaderLang::SourceLocation& sourceLocation) const;
			template<typename T> ValidationResult ComputeExprValue(const ExpressionValue<T>& attribute, ExpressionValue<T>& targetAttribute, const ShaderLang::SourceLocation& sourceLocation);
			template<typename T> std::unique_ptr<T> PropagateConstants(T& node) const;

			void PreregisterIndices(const Module& module);
			void PropagateFunctionFlags(std::size_t funcIndex, FunctionFlags flags, Bitset<>& seen);

			void RegisterBuiltin();

			std::size_t RegisterAlias(std::string name, std::optional<Identifier> aliasData, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t RegisterConstant(std::string name, std::optional<ConstantValue> value, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t RegisterFunction(std::string name, std::optional<FunctionData> funcData, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t RegisterIntrinsic(std::string name, IntrinsicType type);
			std::size_t RegisterModule(std::string moduleIdentifier, std::size_t moduleIndex);
			std::size_t RegisterStruct(std::string name, std::optional<StructDescription*> description, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t RegisterType(std::string name, std::optional<ExpressionType> expressionType, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t RegisterType(std::string name, std::optional<PartialType> partialType, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);
			void RegisterUnresolved(std::string name);
			std::size_t RegisterVariable(std::string name, std::optional<ExpressionType> type, std::optional<std::size_t> index, const ShaderLang::SourceLocation& sourceLocation);

			const Identifier* ResolveAliasIdentifier(const Identifier* identifier, const ShaderLang::SourceLocation& sourceLocation) const;
			void ResolveFunctions();
			std::size_t ResolveStruct(const AliasType& aliasType, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t ResolveStruct(const ExpressionType& exprType, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t ResolveStruct(const StructType& structType, const ShaderLang::SourceLocation& sourceLocation);
			std::size_t ResolveStruct(const UniformType& uniformType, const ShaderLang::SourceLocation& sourceLocation);
			ExpressionType ResolveType(const ExpressionType& exprType, bool resolveAlias, const ShaderLang::SourceLocation& sourceLocation);
			std::optional<ExpressionType> ResolveTypeExpr(const ExpressionValue<ExpressionType>& exprTypeValue, bool resolveAlias, const ShaderLang::SourceLocation& sourceLocation);

			void SanitizeIdentifier(std::string& identifier);
			MultiStatementPtr SanitizeInternal(MultiStatement& rootNode, std::string* error);

			std::string ToString(const ExpressionType& exprType, const ShaderLang::SourceLocation& sourceLocation) const;
			std::string ToString(const NamedPartialType& partialType, const ShaderLang::SourceLocation& sourceLocation) const;
			template<typename... Args> std::string ToString(const std::variant<Args...>& value, const ShaderLang::SourceLocation& sourceLocation) const;

			void TypeMustMatch(const ExpressionType& left, const ExpressionType& right, const ShaderLang::SourceLocation& sourceLocation) const;
			ValidationResult TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right, const ShaderLang::SourceLocation& sourceLocation);

			ValidationResult Validate(DeclareAliasStatement& node);
			ValidationResult Validate(WhileStatement& node);

			ValidationResult Validate(AccessIndexExpression& node);
			ValidationResult Validate(AssignExpression& node);
			ValidationResult Validate(BinaryExpression& node);
			ValidationResult Validate(CallFunctionExpression& node);
			ValidationResult Validate(CastExpression& node);
			ValidationResult Validate(DeclareVariableStatement& node);
			ValidationResult Validate(IntrinsicExpression& node);
			ValidationResult Validate(SwizzleExpression& node);
			ValidationResult Validate(UnaryExpression& node);
			ValidationResult Validate(VariableValueExpression& node);
			ExpressionType ValidateBinaryOp(BinaryType op, const ExpressionType& leftExprType, const ExpressionType& rightExprType, const ShaderLang::SourceLocation& sourceLocation);

			template<std::size_t N> ValidationResult ValidateIntrinsicParamCount(IntrinsicExpression& node);
			ValidationResult ValidateIntrinsicParamMatchingType(IntrinsicExpression& node);
			template<std::size_t N, typename F> ValidationResult ValidateIntrinsicParameter(IntrinsicExpression& node, F&& func);
			template<std::size_t N, typename F> ValidationResult ValidateIntrinsicParameterType(IntrinsicExpression& node, F&& func, const char* typeStr);

			static Expression& MandatoryExpr(const ExpressionPtr& node, const ShaderLang::SourceLocation& sourceLocation);
			static Statement& MandatoryStatement(const StatementPtr& node, const ShaderLang::SourceLocation& sourceLocation);

			static StatementPtr Unscope(StatementPtr node);

			static UInt32 ToSwizzleIndex(char c, const ShaderLang::SourceLocation& sourceLocation);

			enum class IdentifierCategory
			{
				Alias,
				Constant,
				Function,
				Intrinsic,
				Module,
				Struct,
				Type,
				Unresolved,
				Variable
			};

			enum class ValidationResult
			{
				Validated,
				Unresolved
			};

			struct FunctionData
			{
				Bitset<> calledByFunctions;
				DeclareFunctionStatement* node;
				FunctionFlags flags;
			};

			struct IdentifierData
			{
				std::size_t index;
				IdentifierCategory category;
				bool isConditional = false;
			};

			struct Identifier
			{
				std::string name;
				IdentifierData target;
			};

			struct Context;
			Context* m_context;
	};

	inline ModulePtr Sanitize(const Module& module, std::string* error = nullptr);
	inline ModulePtr Sanitize(const Module& module, const SanitizeVisitor::Options& options, std::string* error = nullptr);
}

#include <Nazara/Shader/Ast/SanitizeVisitor.inl>

#endif // NAZARA_SHADER_AST_SANITIZEVISITOR_HPP
