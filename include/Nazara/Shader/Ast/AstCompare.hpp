// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTCOMPARE_HPP
#define NAZARA_SHADER_AST_ASTCOMPARE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Attribute.hpp>
#include <Nazara/Shader/Ast/Module.hpp>
#include <vector>

namespace Nz::ShaderAst
{
	inline bool Compare(const Expression& lhs, const Expression& rhs);
	inline bool Compare(const Module& lhs, const Module& rhs);
	inline bool Compare(const Module::ImportedModule& lhs, const Module::ImportedModule& rhs);
	inline bool Compare(const Module::Metadata& lhs, const Module::Metadata& rhs);
	inline bool Compare(const Statement& lhs, const Statement& rhs);

	template<typename T> bool Compare(const T& lhs, const T& rhs);
	template<typename T, std::size_t S> bool Compare(const std::array<T, S>& lhs, const std::array<T, S>& rhs);
	template<typename T> bool Compare(const std::vector<T>& lhs, const std::vector<T>& rhs);
	template<typename T> bool Compare(const std::unique_ptr<T>& lhs, const std::unique_ptr<T>& rhs);
	template<typename T> bool Compare(const ExpressionValue<T>& lhs, const ExpressionValue<T>& rhs);
	inline bool Compare(const BranchStatement::ConditionalStatement& lhs, const BranchStatement::ConditionalStatement& rhs);
	inline bool Compare(const DeclareExternalStatement::ExternalVar& lhs, const DeclareExternalStatement::ExternalVar& rhs);
	inline bool Compare(const DeclareFunctionStatement::Parameter& lhs, const DeclareFunctionStatement::Parameter& rhs);
	inline bool Compare(const StructDescription& lhs, const StructDescription& rhs);
	inline bool Compare(const StructDescription::StructMember& lhs, const StructDescription::StructMember& rhs);

	inline bool Compare(const AccessIdentifierExpression& lhs, const AccessIdentifierExpression& rhs);
	inline bool Compare(const AccessIndexExpression& lhs, const AccessIndexExpression& rhs);
	inline bool Compare(const AssignExpression& lhs, const AssignExpression& rhs);
	inline bool Compare(const BinaryExpression& lhs, const BinaryExpression& rhs);
	inline bool Compare(const CallFunctionExpression& lhs, const CallFunctionExpression& rhs);
	inline bool Compare(const CallMethodExpression& lhs, const CallMethodExpression& rhs);
	inline bool Compare(const CastExpression& lhs, const CastExpression& rhs);
	inline bool Compare(const ConditionalExpression& lhs, const ConditionalExpression& rhs);
	inline bool Compare(const ConstantExpression& lhs, const ConstantExpression& rhs);
	inline bool Compare(const ConstantValueExpression& lhs, const ConstantValueExpression& rhs);
	inline bool Compare(const FunctionExpression& lhs, const FunctionExpression& rhs);
	inline bool Compare(const IdentifierExpression& lhs, const IdentifierExpression& rhs);
	inline bool Compare(const IntrinsicExpression& lhs, const IntrinsicExpression& rhs);
	inline bool Compare(const IntrinsicFunctionExpression& lhs, const IntrinsicFunctionExpression& rhs);
	inline bool Compare(const StructTypeExpression& lhs, const StructTypeExpression& rhs);
	inline bool Compare(const SwizzleExpression& lhs, const SwizzleExpression& rhs);
	inline bool Compare(const VariableExpression& lhs, const VariableExpression& rhs);
	inline bool Compare(const UnaryExpression& lhs, const UnaryExpression& rhs);

	inline bool Compare(const BranchStatement& lhs, const BranchStatement& rhs);
	inline bool Compare(const ConditionalStatement& lhs, const ConditionalStatement& rhs);
	inline bool Compare(const DeclareAliasStatement& lhs, const DeclareAliasStatement& rhs);
	inline bool Compare(const DeclareConstStatement& lhs, const DeclareConstStatement& rhs);
	inline bool Compare(const DeclareExternalStatement& lhs, const DeclareExternalStatement& rhs);
	inline bool Compare(const DeclareFunctionStatement& lhs, const DeclareFunctionStatement& rhs);
	inline bool Compare(const DeclareOptionStatement& lhs, const DeclareOptionStatement& rhs);
	inline bool Compare(const DeclareStructStatement& lhs, const DeclareStructStatement& rhs);
	inline bool Compare(const DeclareVariableStatement& lhs, const DeclareVariableStatement& rhs);
	inline bool Compare(const DiscardStatement& lhs, const DiscardStatement& rhs);
	inline bool Compare(const ExpressionStatement& lhs, const ExpressionStatement& rhs);
	inline bool Compare(const ForStatement& lhs, const ForStatement& rhs);
	inline bool Compare(const ForEachStatement& lhs, const ForEachStatement& rhs);
	inline bool Compare(const ImportStatement& lhs, const ImportStatement& rhs);
	inline bool Compare(const MultiStatement& lhs, const MultiStatement& rhs);
	inline bool Compare(const NoOpStatement& lhs, const NoOpStatement& rhs);
	inline bool Compare(const ReturnStatement& lhs, const ReturnStatement& rhs);
	inline bool Compare(const ScopedStatement& lhs, const ScopedStatement& rhs);
	inline bool Compare(const WhileStatement& lhs, const WhileStatement& rhs);
}

#include <Nazara/Shader/Ast/AstCompare.inl>

#endif // NAZARA_SHADER_AST_ASTCOMPARE_HPP
