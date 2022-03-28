// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_SHADERBUILDER_HPP
#define NAZARA_SHADER_SHADERBUILDER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <array>
#include <memory>
#include <optional>

namespace Nz::ShaderBuilder
{
	namespace Impl
	{
		struct AccessIndex
		{
			inline ShaderAst::AccessIndexExpressionPtr operator()(ShaderAst::ExpressionPtr expr, Int32 index) const;
			inline ShaderAst::AccessIndexExpressionPtr operator()(ShaderAst::ExpressionPtr expr, const std::vector<Int32>& indexConstants) const;
			inline ShaderAst::AccessIndexExpressionPtr operator()(ShaderAst::ExpressionPtr expr, ShaderAst::ExpressionPtr indexExpression) const;
			inline ShaderAst::AccessIndexExpressionPtr operator()(ShaderAst::ExpressionPtr expr, std::vector<ShaderAst::ExpressionPtr> indexExpressions) const;
		};

		struct AccessMember
		{
			inline ShaderAst::AccessIdentifierExpressionPtr operator()(ShaderAst::ExpressionPtr expr, std::vector<std::string> memberIdentifiers) const;
		};

		struct Assign
		{
			inline ShaderAst::AssignExpressionPtr operator()(ShaderAst::AssignType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const;
		};

		struct Binary
		{
			inline ShaderAst::BinaryExpressionPtr operator()(ShaderAst::BinaryType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const;
		};

		template<bool Const>
		struct Branch
		{
			inline ShaderAst::BranchStatementPtr operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr truePath, ShaderAst::StatementPtr falsePath = nullptr) const;
			inline ShaderAst::BranchStatementPtr operator()(std::vector<ShaderAst::BranchStatement::ConditionalStatement> condStatements, ShaderAst::StatementPtr elseStatement = nullptr) const;
		};

		struct CallFunction
		{
			inline ShaderAst::CallFunctionExpressionPtr operator()(std::string functionName, std::vector<ShaderAst::ExpressionPtr> parameters) const;
			inline ShaderAst::CallFunctionExpressionPtr operator()(ShaderAst::ExpressionPtr functionExpr, std::vector<ShaderAst::ExpressionPtr> parameters) const;
		};

		struct Cast
		{
			inline ShaderAst::CastExpressionPtr operator()(ShaderAst::ExpressionValue<ShaderAst::ExpressionType> targetType, ShaderAst::ExpressionPtr expression) const;
			inline ShaderAst::CastExpressionPtr operator()(ShaderAst::ExpressionValue<ShaderAst::ExpressionType> targetType, std::array<ShaderAst::ExpressionPtr, 4> expressions) const;
			inline ShaderAst::CastExpressionPtr operator()(ShaderAst::ExpressionValue<ShaderAst::ExpressionType> targetType, std::vector<ShaderAst::ExpressionPtr> expressions) const;
		};

		struct ConditionalExpression
		{
			inline ShaderAst::ConditionalExpressionPtr operator()(ShaderAst::ExpressionPtr condition, ShaderAst::ExpressionPtr truePath, ShaderAst::ExpressionPtr falsePath) const;
		};

		struct ConditionalStatement
		{
			inline ShaderAst::ConditionalStatementPtr operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr statement) const;
		};

		struct Constant
		{
			inline ShaderAst::ConstantValueExpressionPtr operator()(ShaderAst::ConstantValue value) const;
			template<typename T> ShaderAst::ConstantValueExpressionPtr operator()(ShaderAst::ExpressionType type, T value) const;
		};

		struct DeclareAlias
		{
			inline ShaderAst::DeclareAliasStatementPtr operator()(std::string name, ShaderAst::ExpressionPtr expression) const;
		};

		struct DeclareConst
		{
			inline ShaderAst::DeclareConstStatementPtr operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const;
			inline ShaderAst::DeclareConstStatementPtr operator()(std::string name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct DeclareFunction
		{
			inline ShaderAst::DeclareFunctionStatementPtr operator()(std::string name, ShaderAst::StatementPtr statement) const;
			inline ShaderAst::DeclareFunctionStatementPtr operator()(std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> returnType = ShaderAst::ExpressionType{ ShaderAst::NoType{} }) const;
			inline ShaderAst::DeclareFunctionStatementPtr operator()(std::optional<ShaderStageType> entryStage, std::string name, ShaderAst::StatementPtr statement) const;
			inline ShaderAst::DeclareFunctionStatementPtr operator()(std::optional<ShaderStageType> entryStage, std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> returnType = ShaderAst::ExpressionType{ ShaderAst::NoType{} }) const;
		};

		struct DeclareOption
		{
			inline ShaderAst::DeclareOptionStatementPtr operator()(std::string name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct DeclareStruct
		{
			inline ShaderAst::DeclareStructStatementPtr operator()(ShaderAst::StructDescription description, ShaderAst::ExpressionValue<bool> isExported) const;
		};

		struct DeclareVariable
		{
			inline ShaderAst::DeclareVariableStatementPtr operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const;
			inline ShaderAst::DeclareVariableStatementPtr operator()(std::string name, ShaderAst::ExpressionValue<ShaderAst::ExpressionType> type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct ExpressionStatement
		{
			inline ShaderAst::ExpressionStatementPtr operator()(ShaderAst::ExpressionPtr expression) const;
		};

		struct For
		{
			inline ShaderAst::ForStatementPtr operator()(std::string varName, ShaderAst::ExpressionPtr fromExpression, ShaderAst::ExpressionPtr toExpression, ShaderAst::StatementPtr statement) const;
			inline ShaderAst::ForStatementPtr operator()(std::string varName, ShaderAst::ExpressionPtr fromExpression, ShaderAst::ExpressionPtr toExpression, ShaderAst::ExpressionPtr stepExpression, ShaderAst::StatementPtr statement) const;
		};

		struct ForEach
		{
			inline ShaderAst::ForEachStatementPtr operator()(std::string varName, ShaderAst::ExpressionPtr expression, ShaderAst::StatementPtr statement) const;
		};

		struct Function
		{
			inline ShaderAst::FunctionExpressionPtr operator()(std::size_t funcId) const;
		};

		struct Identifier
		{
			inline ShaderAst::IdentifierExpressionPtr operator()(std::string name) const;
		};

		struct Import
		{
			inline ShaderAst::ImportStatementPtr operator()(std::string modulePath) const;
		};

		struct Intrinsic
		{
			inline ShaderAst::IntrinsicExpressionPtr operator()(ShaderAst::IntrinsicType intrinsicType, std::vector<ShaderAst::ExpressionPtr> parameters) const;
		};

		struct IntrinsicFunction
		{
			inline ShaderAst::IntrinsicFunctionExpressionPtr operator()(std::size_t intrinsicFunctionId, ShaderAst::IntrinsicType intrinsicType) const;
		};

		struct Multi
		{
			inline ShaderAst::MultiStatementPtr operator()(std::vector<ShaderAst::StatementPtr> statements = {}) const;
		};

		template<typename T>
		struct NoParam
		{
			inline std::unique_ptr<T> operator()() const;
		};

		struct Return
		{
			inline ShaderAst::ReturnStatementPtr operator()(ShaderAst::ExpressionPtr expr = nullptr) const;
		};

		struct Scoped
		{
			inline ShaderAst::ScopedStatementPtr operator()(ShaderAst::StatementPtr statement) const;
		};

		struct StructType
		{
			inline ShaderAst::StructTypeExpressionPtr operator()(std::size_t structTypeId) const;
		};

		struct Swizzle
		{
			inline ShaderAst::SwizzleExpressionPtr operator()(ShaderAst::ExpressionPtr expression, std::array<UInt32, 4> swizzleComponents, std::size_t componentCount) const;
			inline ShaderAst::SwizzleExpressionPtr operator()(ShaderAst::ExpressionPtr expression, std::vector<UInt32> swizzleComponents) const;
		};

		struct Unary
		{
			inline ShaderAst::UnaryExpressionPtr operator()(ShaderAst::UnaryType op, ShaderAst::ExpressionPtr expression) const;
		};

		struct Variable
		{
			inline ShaderAst::VariableValueExpressionPtr operator()(std::size_t variableId, ShaderAst::ExpressionType expressionType) const;
		};

		struct While
		{
			inline ShaderAst::WhileStatementPtr operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr body) const;
		};
	}

	constexpr Impl::AccessIndex AccessIndex;
	constexpr Impl::AccessMember AccessMember;
	constexpr Impl::Assign Assign;
	constexpr Impl::Binary Binary;
	constexpr Impl::Branch<false> Branch;
	constexpr Impl::CallFunction CallFunction;
	constexpr Impl::Cast Cast;
	constexpr Impl::ConditionalExpression ConditionalExpression;
	constexpr Impl::ConditionalStatement ConditionalStatement;
	constexpr Impl::Constant Constant;
	constexpr Impl::Branch<true> ConstBranch;
	constexpr Impl::DeclareAlias DeclareAlias;
	constexpr Impl::DeclareConst DeclareConst;
	constexpr Impl::DeclareFunction DeclareFunction;
	constexpr Impl::DeclareOption DeclareOption;
	constexpr Impl::DeclareStruct DeclareStruct;
	constexpr Impl::DeclareVariable DeclareVariable;
	constexpr Impl::ExpressionStatement ExpressionStatement;
	constexpr Impl::NoParam<ShaderAst::DiscardStatement> Discard;
	constexpr Impl::For For;
	constexpr Impl::ForEach ForEach;
	constexpr Impl::Function Function;
	constexpr Impl::Identifier Identifier;
	constexpr Impl::IntrinsicFunction IntrinsicFunction;
	constexpr Impl::Import Import;
	constexpr Impl::Intrinsic Intrinsic;
	constexpr Impl::Multi MultiStatement;
	constexpr Impl::NoParam<ShaderAst::NoOpStatement> NoOp;
	constexpr Impl::Return Return;
	constexpr Impl::Scoped Scoped;
	constexpr Impl::StructType StructType;
	constexpr Impl::Swizzle Swizzle;
	constexpr Impl::Unary Unary;
	constexpr Impl::Variable Variable;
	constexpr Impl::While While;
}

#include <Nazara/Shader/ShaderBuilder.inl>

#endif // NAZARA_SHADER_SHADERBUILDER_HPP
