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
	template<ShaderNodes::AssignType op>
	struct AssignOpBuilder
	{
		constexpr AssignOpBuilder() = default;

		std::shared_ptr<ShaderNodes::AssignOp> operator()(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right) const;
	};

	template<ShaderNodes::BinaryType op>
	struct BinOpBuilder
	{
		constexpr BinOpBuilder() = default;

		std::shared_ptr<ShaderNodes::BinaryOp> operator()(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right) const;
	};

	struct BuiltinBuilder
	{
		constexpr BuiltinBuilder() = default;

		inline std::shared_ptr<ShaderNodes::Variable> operator()(ShaderNodes::BuiltinEntry builtin) const;
	};

	template<typename T>
	struct GenBuilder
	{
		constexpr GenBuilder() = default;

		template<typename... Args> std::shared_ptr<T> operator()(Args&&... args) const;
	};

	constexpr GenBuilder<ShaderNodes::AccessMember> AccessMember;
	constexpr BinOpBuilder<ShaderNodes::BinaryType::Add> Add;
	constexpr AssignOpBuilder<ShaderNodes::AssignType::Simple> Assign;
	constexpr BuiltinBuilder Builtin;
	constexpr GenBuilder<ShaderNodes::StatementBlock> Block;
	constexpr GenBuilder<ShaderNodes::Branch> Branch;
	constexpr GenBuilder<ShaderNodes::ConditionalExpression> ConditionalExpression;
	constexpr GenBuilder<ShaderNodes::ConditionalStatement> ConditionalStatement;
	constexpr GenBuilder<ShaderNodes::Constant> Constant;
	constexpr GenBuilder<ShaderNodes::DeclareVariable> DeclareVariable;
	constexpr GenBuilder<ShaderNodes::Discard> Discard;
	constexpr BinOpBuilder<ShaderNodes::BinaryType::Divide> Divide;
	constexpr BinOpBuilder<ShaderNodes::BinaryType::Equality> Equal;
	constexpr GenBuilder<ShaderNodes::ExpressionStatement> ExprStatement;
	constexpr GenBuilder<ShaderNodes::Identifier> Identifier;
	constexpr GenBuilder<ShaderNodes::IntrinsicCall> IntrinsicCall;
	constexpr GenBuilder<ShaderNodes::InputVariable> Input;
	constexpr GenBuilder<ShaderNodes::LocalVariable> Local;
	constexpr BinOpBuilder<ShaderNodes::BinaryType::Multiply> Multiply;
	constexpr GenBuilder<ShaderNodes::OutputVariable> Output;
	constexpr GenBuilder<ShaderNodes::ParameterVariable> Parameter;
	constexpr GenBuilder<ShaderNodes::Sample2D> Sample2D;
	constexpr GenBuilder<ShaderNodes::SwizzleOp> Swizzle;
	constexpr BinOpBuilder<ShaderNodes::BinaryType::Substract> Substract;
	constexpr GenBuilder<ShaderNodes::UniformVariable> Uniform;

	template<ShaderNodes::BasicType Type, typename... Args> std::shared_ptr<ShaderNodes::Cast> Cast(Args&&... args);
}

#include <Nazara/Shader/ShaderBuilder.inl>

#endif // NAZARA_SHADER_BUILDER_HPP
