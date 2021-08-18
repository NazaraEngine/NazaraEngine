// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_BUILDER_HPP
#define NAZARA_SHADER_BUILDER_HPP

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
			inline std::unique_ptr<ShaderAst::AccessIndexExpression> operator()(ShaderAst::ExpressionPtr expr, const std::vector<Int32>& indexConstants) const;
			inline std::unique_ptr<ShaderAst::AccessIndexExpression> operator()(ShaderAst::ExpressionPtr expr, std::vector<ShaderAst::ExpressionPtr> indexExpressions) const;
		};

		struct AccessMember
		{
			inline std::unique_ptr<ShaderAst::AccessIdentifierExpression> operator()(ShaderAst::ExpressionPtr expr, std::vector<std::string> memberIdentifiers) const;
		};

		struct Assign
		{
			inline std::unique_ptr<ShaderAst::AssignExpression> operator()(ShaderAst::AssignType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const;
		};

		struct Binary
		{
			inline std::unique_ptr<ShaderAst::BinaryExpression> operator()(ShaderAst::BinaryType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const;
		};

		template<bool Const>
		struct Branch
		{
			inline std::unique_ptr<ShaderAst::BranchStatement> operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr truePath, ShaderAst::StatementPtr falsePath = nullptr) const;
			inline std::unique_ptr<ShaderAst::BranchStatement> operator()(std::vector<ShaderAst::BranchStatement::ConditionalStatement> condStatements, ShaderAst::StatementPtr elseStatement = nullptr) const;
		};

		struct CallFunction
		{
			inline std::unique_ptr<ShaderAst::CallFunctionExpression> operator()(std::string functionName, std::vector<ShaderAst::ExpressionPtr> parameters) const;
		};

		struct Cast
		{
			inline std::unique_ptr<ShaderAst::CastExpression> operator()(ShaderAst::ExpressionType targetType, std::array<ShaderAst::ExpressionPtr, 4> expressions) const;
			inline std::unique_ptr<ShaderAst::CastExpression> operator()(ShaderAst::ExpressionType targetType, std::vector<ShaderAst::ExpressionPtr> expressions) const;
		};

		struct ConditionalExpression
		{
			inline std::unique_ptr<ShaderAst::ConditionalExpression> operator()(ShaderAst::ExpressionPtr condition, ShaderAst::ExpressionPtr truePath, ShaderAst::ExpressionPtr falsePath) const;
		};

		struct ConditionalStatement
		{
			inline std::unique_ptr<ShaderAst::ConditionalStatement> operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr statement) const;
		};

		struct Constant
		{
			inline std::unique_ptr<ShaderAst::ConstantValueExpression> operator()(ShaderAst::ConstantValue value) const;
		};

		struct DeclareConst
		{
			inline std::unique_ptr<ShaderAst::DeclareConstStatement> operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const;
			inline std::unique_ptr<ShaderAst::DeclareConstStatement> operator()(std::string name, ShaderAst::ExpressionType type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct DeclareFunction
		{
			inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> operator()(std::string name, ShaderAst::StatementPtr statement) const;
			inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> operator()(std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionType returnType = ShaderAst::NoType{}) const;
			inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> operator()(std::optional<ShaderStageType> entryStage, std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ExpressionType returnType = ShaderAst::NoType{}) const;
		};

		struct DeclareOption
		{
			inline std::unique_ptr<ShaderAst::DeclareOptionStatement> operator()(std::string name, ShaderAst::ExpressionType type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct DeclareStruct
		{
			inline std::unique_ptr<ShaderAst::DeclareStructStatement> operator()(ShaderAst::StructDescription description) const;
		};

		struct DeclareVariable
		{
			inline std::unique_ptr<ShaderAst::DeclareVariableStatement> operator()(std::string name, ShaderAst::ExpressionPtr initialValue) const;
			inline std::unique_ptr<ShaderAst::DeclareVariableStatement> operator()(std::string name, ShaderAst::ExpressionType type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct ExpressionStatement
		{
			inline std::unique_ptr<ShaderAst::ExpressionStatement> operator()(ShaderAst::ExpressionPtr expression) const;
		};

		struct Identifier
		{
			inline std::unique_ptr<ShaderAst::IdentifierExpression> operator()(std::string name) const;
		};

		struct Intrinsic
		{
			inline std::unique_ptr<ShaderAst::IntrinsicExpression> operator()(ShaderAst::IntrinsicType intrinsicType, std::vector<ShaderAst::ExpressionPtr> parameters) const;
		};

		struct Multi
		{
			inline std::unique_ptr<ShaderAst::MultiStatement> operator()(std::vector<ShaderAst::StatementPtr> statements = {}) const;
		};

		template<typename T>
		struct NoParam
		{
			inline std::unique_ptr<T> operator()() const;
		};

		struct Return
		{
			inline std::unique_ptr<ShaderAst::ReturnStatement> operator()(ShaderAst::ExpressionPtr expr = nullptr) const;
		};

		struct Swizzle
		{
			inline std::unique_ptr<ShaderAst::SwizzleExpression> operator()(ShaderAst::ExpressionPtr expression, std::vector<ShaderAst::SwizzleComponent> swizzleComponents) const;
		};

		struct Unary
		{
			inline std::unique_ptr<ShaderAst::UnaryExpression> operator()(ShaderAst::UnaryType op, ShaderAst::ExpressionPtr expression) const;
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
	constexpr Impl::DeclareConst DeclareConst;
	constexpr Impl::DeclareFunction DeclareFunction;
	constexpr Impl::DeclareOption DeclareOption;
	constexpr Impl::DeclareStruct DeclareStruct;
	constexpr Impl::DeclareVariable DeclareVariable;
	constexpr Impl::ExpressionStatement ExpressionStatement;
	constexpr Impl::NoParam<ShaderAst::DiscardStatement> Discard;
	constexpr Impl::Identifier Identifier;
	constexpr Impl::Intrinsic Intrinsic;
	constexpr Impl::Multi MultiStatement;
	constexpr Impl::NoParam<ShaderAst::NoOpStatement> NoOp;
	constexpr Impl::Return Return;
	constexpr Impl::Swizzle Swizzle;
	constexpr Impl::Unary Unary;
}

#include <Nazara/Shader/ShaderBuilder.inl>

#endif // NAZARA_SHADER_BUILDER_HPP
