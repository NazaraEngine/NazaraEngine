// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_BUILDER_HPP
#define NAZARA_SHADER_BUILDER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/ShaderNodes.hpp>
#include <memory>

namespace Nz::ShaderBuilder
{
	namespace Impl
	{
		struct Binary
		{
			inline std::unique_ptr<ShaderAst::BinaryExpression> operator()(ShaderAst::BinaryType op, ShaderAst::ExpressionPtr left, ShaderAst::ExpressionPtr right) const;
		};

		struct Branch
		{
			inline std::unique_ptr<ShaderAst::BranchStatement> operator()(ShaderAst::ExpressionPtr condition, ShaderAst::StatementPtr truePath, ShaderAst::StatementPtr falsePath = nullptr) const;
			inline std::unique_ptr<ShaderAst::BranchStatement> operator()(std::vector<ShaderAst::BranchStatement::ConditionalStatement> condStatements, ShaderAst::StatementPtr elseStatement = nullptr) const;
		};

		struct Constant
		{
			inline std::unique_ptr<ShaderAst::ConstantExpression> operator()(ShaderConstantValue value) const;
		};

		struct DeclareFunction
		{
			inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> operator()(std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ShaderExpressionType returnType = ShaderAst::BasicType::Void) const;
			inline std::unique_ptr<ShaderAst::DeclareFunctionStatement> operator()(std::vector<ShaderAst::Attribute> attributes, std::string name, std::vector<ShaderAst::DeclareFunctionStatement::Parameter> parameters, std::vector<ShaderAst::StatementPtr> statements, ShaderAst::ShaderExpressionType returnType = ShaderAst::BasicType::Void) const;
		};

		struct DeclareVariable
		{
			inline std::unique_ptr<ShaderAst::DeclareVariableStatement> operator()(std::string name, ShaderAst::ShaderExpressionType type, ShaderAst::ExpressionPtr initialValue = nullptr) const;
		};

		struct Identifier
		{
			inline std::unique_ptr<ShaderAst::IdentifierExpression> operator()(std::string name) const;
		};

		struct Return
		{
			inline std::unique_ptr<ShaderAst::ReturnStatement> operator()(ShaderAst::ExpressionPtr expr = nullptr) const;
		};

		template<typename T>
		struct NoParam
		{
			std::unique_ptr<T> operator()() const;
		};
	}

	constexpr Impl::Binary Binary;
	constexpr Impl::Branch Branch;
	constexpr Impl::Constant Constant;
	constexpr Impl::DeclareFunction DeclareFunction;
	constexpr Impl::DeclareVariable DeclareVariable;
	constexpr Impl::NoParam<ShaderAst::DiscardStatement> Discard;
	constexpr Impl::Identifier Identifier;
	constexpr Impl::NoParam<ShaderAst::NoOpStatement> NoOp;
	constexpr Impl::Return Return;
}

#include <Nazara/Shader/ShaderBuilder.inl>

#endif // NAZARA_SHADER_BUILDER_HPP
