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
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API SanitizeVisitor final : AstCloner
	{
		public:
			struct Options;

			SanitizeVisitor() = default;
			SanitizeVisitor(const SanitizeVisitor&) = delete;
			SanitizeVisitor(SanitizeVisitor&&) = delete;
			~SanitizeVisitor() = default;

			inline StatementPtr Sanitize(Statement& statement, std::string* error = nullptr);
			StatementPtr Sanitize(Statement& statement, const Options& options, std::string* error = nullptr);

			SanitizeVisitor& operator=(const SanitizeVisitor&) = delete;
			SanitizeVisitor& operator=(SanitizeVisitor&&) = delete;

			static UInt32 ToSwizzleIndex(char c);

			struct Options
			{
				std::unordered_set<std::string> reservedIdentifiers;
				std::unordered_map<std::size_t, ConstantValue> optionValues;
				bool makeVariableNameUnique = false;
				bool removeConstDeclaration = false;
				bool reduceLoopsToWhile = false;
				bool removeCompoundAssignments = false;
				bool removeOptionDeclaration = false;
				bool removeScalarSwizzling = false;
				bool splitMultipleBranches = false;
				bool useIdentifierAccessesForStructs = true;
			};

		private:
			struct FunctionData;
			struct Identifier;

			using AstCloner::CloneExpression;

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
			StatementPtr Clone(MultiStatement& node) override;
			StatementPtr Clone(WhileStatement& node) override;

			const Identifier* FindIdentifier(const std::string_view& identifierName) const;

			Expression& MandatoryExpr(const ExpressionPtr& node);
			Statement& MandatoryStatement(const StatementPtr& node);
			void TypeMustMatch(const ExpressionPtr& left, const ExpressionPtr& right);
			void TypeMustMatch(const ExpressionType& left, const ExpressionType& right);

			void PushScope();
			void PopScope();

			ExpressionPtr CacheResult(ExpressionPtr expression);

			template<typename T> const T& ComputeAttributeValue(AttributeValue<T>& attribute);
			ConstantValue ComputeConstantValue(Expression& expr);
			template<typename T> std::unique_ptr<T> Optimize(T& node);

			std::size_t DeclareFunction(DeclareFunctionStatement& funcDecl);

			void PropagateFunctionFlags(std::size_t funcIndex, FunctionFlags flags, Bitset<>& seen);

			std::size_t RegisterConstant(std::string name, ConstantValue value);
			FunctionData& RegisterFunction(std::size_t functionIndex);
			std::size_t RegisterIntrinsic(std::string name, IntrinsicType type);
			std::size_t RegisterStruct(std::string name, StructDescription* description);
			std::size_t RegisterVariable(std::string name, ExpressionType type);

			void ResolveFunctions();

			std::size_t ResolveStruct(const ExpressionType& exprType);
			std::size_t ResolveStruct(const IdentifierType& identifierType);
			std::size_t ResolveStruct(const StructType& structType);
			std::size_t ResolveStruct(const UniformType& uniformType);
			ExpressionType ResolveType(const ExpressionType& exprType);

			void SanitizeIdentifier(std::string& identifier);

			void Validate(WhileStatement& node);

			void Validate(AccessIndexExpression& node);
			void Validate(AssignExpression& node);
			void Validate(BinaryExpression& node);
			void Validate(CallFunctionExpression& node, const DeclareFunctionStatement* referenceDeclaration);
			void Validate(CastExpression& node);
			void Validate(DeclareVariableStatement& node);
			void Validate(IntrinsicExpression& node);
			void Validate(SwizzleExpression& node);
			void Validate(UnaryExpression& node);
			void Validate(VariableExpression& node);
			ExpressionType ValidateBinaryOp(BinaryType op, const ExpressionPtr& leftExpr, const ExpressionPtr& rightExpr);

			struct FunctionData
			{
				Bitset<> calledByFunctions;
				DeclareFunctionStatement* node;
				FunctionFlags flags;
				bool defined = false;
			};

			struct Identifier
			{
				enum class Type
				{
					Alias,
					Constant,
					Function,
					Intrinsic,
					Struct,
					Variable
				};

				std::string name;
				std::size_t index;
				Type type;
			};

			struct Context;
			Context* m_context;
	};

	inline StatementPtr Sanitize(Statement& ast, std::string* error = nullptr);
	inline StatementPtr Sanitize(Statement& ast, const SanitizeVisitor::Options& options, std::string* error = nullptr);
}

#include <Nazara/Shader/Ast/SanitizeVisitor.inl>

#endif // NAZARA_SHADER_AST_SANITIZEVISITOR_HPP
