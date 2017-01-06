// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_BUILDER_HPP
#define NAZARA_SHADER_BUILDER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <memory>

namespace Nz { namespace ShaderBuilder
{
	template<ShaderAst::AssignType op>
	struct AssignOpBuilder
	{
		constexpr AssignOpBuilder() = default;

		std::shared_ptr<ShaderAst::AssignOp> operator()(const ShaderAst::VariablePtr& left, const ShaderAst::ExpressionPtr& right) const;
	};

	template<ShaderAst::BinaryType op>
	struct BinOpBuilder
	{
		constexpr BinOpBuilder() = default;

		std::shared_ptr<ShaderAst::BinaryOp> operator()(const ShaderAst::ExpressionPtr& left, const ShaderAst::ExpressionPtr& right) const;
	};

	struct BuiltinBuilder
	{
		constexpr BuiltinBuilder() = default;

		std::shared_ptr<ShaderAst::Variable> operator()(ShaderAst::BuiltinEntry builtin) const;
	};

	template<typename T>
	struct GenBuilder
	{
		constexpr GenBuilder() = default;

		template<typename... Args> std::shared_ptr<T> operator()(Args&&... args) const;
	};

	template<ShaderAst::VariableType type>
	struct VarBuilder
	{
		constexpr VarBuilder() = default;

		template<typename... Args> std::shared_ptr<ShaderAst::Variable> operator()(Args&&... args) const;
	};

	constexpr BinOpBuilder<ShaderAst::BinaryType::Add> Add;
	constexpr AssignOpBuilder<ShaderAst::AssignType::Simple> Assign;
	constexpr BuiltinBuilder Builtin;
	constexpr GenBuilder<ShaderAst::StatementBlock> Block;
	constexpr GenBuilder<ShaderAst::Branch> Branch;
	constexpr GenBuilder<ShaderAst::Constant> Constant;
	constexpr BinOpBuilder<ShaderAst::BinaryType::Divide> Divide;
	constexpr BinOpBuilder<ShaderAst::BinaryType::Equality> Equal;
	constexpr GenBuilder<ShaderAst::ExpressionStatement> ExprStatement;
	constexpr VarBuilder<ShaderAst::VariableType::Input> Input;
	constexpr BinOpBuilder<ShaderAst::BinaryType::Multiply> Multiply;
	constexpr VarBuilder<ShaderAst::VariableType::Output> Output;
	constexpr VarBuilder<ShaderAst::VariableType::Parameter> Parameter;
	constexpr BinOpBuilder<ShaderAst::BinaryType::Substract> Substract;
	constexpr VarBuilder<ShaderAst::VariableType::Uniform> Uniform;
	constexpr VarBuilder<ShaderAst::VariableType::Variable> Variable;

	template<ShaderAst::ExpressionType Type, typename... Args> std::shared_ptr<ShaderAst::Cast> Cast(Args&&... args);
} }

#include <Nazara/Renderer/ShaderBuilder.inl>

#endif // NAZARA_SHADER_BUILDER_HPP
