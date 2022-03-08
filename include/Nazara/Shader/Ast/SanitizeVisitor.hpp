// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_SANITIZEVISITOR_HPP
#define NAZARA_SHADER_AST_SANITIZEVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Shader/Config.hpp>
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

			static UInt32 ToSwizzleIndex(char c);

			struct Options
			{
				std::function<ModulePtr(const std::vector<std::string>& /*modulePath*/)> moduleCallback;
				std::unordered_set<std::string> reservedIdentifiers;
				std::unordered_map<UInt32, ConstantValue> optionValues;
				bool makeVariableNameUnique = false;
				bool reduceLoopsToWhile = false;
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
			struct CurrentFunctionData;
			struct Environment;
			struct FunctionData;
			struct Identifier;
			struct IdentifierData;
			template<typename T> struct IdentifierList;
			struct Scope;

			using AstCloner::CloneExpression;
			ExpressionValue<ExpressionType> CloneType(const ExpressionValue<ExpressionType>& exprType) override;

			ExpressionPtr Clone(AccessIdentifierExpression& node) override;
			ExpressionPtr Clone(AccessIndexExpression& node) override;
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
			ExpressionPtr Clone(VariableExpression& node) override;

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
			TypeParameter FindTypeParameter(const std::string_view& identifierName) const;

			Expression& MandatoryExpr(const ExpressionPtr& node) const;
			Statement& MandatoryStatement(const StatementPtr& node) const;

			void PushScope();
			void PopScope();

			ExpressionPtr CacheResult(ExpressionPtr expression);

			ConstantValue ComputeConstantValue(Expression& expr) const;
			template<typename T> const T& ComputeExprValue(ExpressionValue<T>& attribute) const;
			template<typename T> std::unique_ptr<T> PropagateConstants(T& node) const;

			void PropagateFunctionFlags(std::size_t funcIndex, FunctionFlags flags, Bitset<>& seen);

			void RegisterBuiltin();

			std::size_t RegisterAlias(std::string name, IdentifierData aliasData, std::optional<std::size_t> index = {});
			std::size_t RegisterConstant(std::string name, ConstantValue value, std::optional<std::size_t> index = {});
			std::size_t RegisterFunction(std::string name, FunctionData funcData, std::optional<std::size_t> index = {});
			std::size_t RegisterIntrinsic(std::string name, IntrinsicType type);
			std::size_t RegisterModule(std::string moduleIdentifier, std::size_t moduleIndex);
			std::size_t RegisterStruct(std::string name, StructDescription* description, std::optional<std::size_t> index = {});
			std::size_t RegisterType(std::string name, ExpressionType expressionType, std::optional<std::size_t> index = {});
			std::size_t RegisterType(std::string name, PartialType partialType, std::optional<std::size_t> index = {});
			std::size_t RegisterVariable(std::string name, ExpressionType type, std::optional<std::size_t> index = {});

			const IdentifierData* ResolveAlias(const IdentifierData* identifier) const;
			void ResolveFunctions();
			const ExpressionPtr& ResolveCondExpression(ConditionalExpression& node);
			std::size_t ResolveStruct(const ExpressionType& exprType);
			std::size_t ResolveStruct(const IdentifierType& identifierType);
			std::size_t ResolveStruct(const StructType& structType);
			std::size_t ResolveStruct(const UniformType& uniformType);
			ExpressionType ResolveType(const ExpressionType& exprType);
			ExpressionType ResolveType(const ExpressionValue<ExpressionType>& exprTypeValue);

			void SanitizeIdentifier(std::string& identifier);
			MultiStatementPtr SanitizeInternal(MultiStatement& rootNode, std::string* error);

			void TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right) const;
			void TypeMustMatch(const ExpressionType& left, const ExpressionType& right) const;

			StatementPtr Unscope(StatementPtr node);

			void Validate(DeclareAliasStatement& node);
			void Validate(WhileStatement& node);

			void Validate(AccessIndexExpression& node);
			void Validate(AssignExpression& node);
			void Validate(BinaryExpression& node);
			void Validate(CallFunctionExpression& node);
			void Validate(CastExpression& node);
			void Validate(DeclareVariableStatement& node);
			void Validate(IntrinsicExpression& node);
			void Validate(SwizzleExpression& node);
			void Validate(UnaryExpression& node);
			void Validate(VariableExpression& node);
			ExpressionType ValidateBinaryOp(BinaryType op, const ExpressionPtr& leftExpr, const ExpressionPtr& rightExpr);

			enum class IdentifierCategory
			{
				Alias,
				Constant,
				Function,
				Intrinsic,
				Module,
				Struct,
				Type,
				Variable
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
			};

			struct Identifier
			{
				std::string name;
				IdentifierData data;
			};

			struct Context;
			Context* m_context;
	};

	inline ModulePtr Sanitize(const Module& module, std::string* error = nullptr);
	inline ModulePtr Sanitize(const Module& module, const SanitizeVisitor::Options& options, std::string* error = nullptr);
}

#include <Nazara/Shader/Ast/SanitizeVisitor.inl>

#endif // NAZARA_SHADER_AST_SANITIZEVISITOR_HPP
