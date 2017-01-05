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
	template<typename T>
	class GenBuilder
	{
		public:
			template<typename... Args>
			std::shared_ptr<T> operator()(Args&&... args) const
			{
				return std::make_shared<T>(std::forward<Args>(args)...);
			}
	};

	template<Nz::ShaderAst::AssignType op>
	class AssignOpBuilder
	{
		public:
			std::shared_ptr<Nz::ShaderAst::AssignOp> operator()(const Nz::ShaderAst::VariablePtr& left, const Nz::ShaderAst::ExpressionPtr& right) const
			{
				return std::make_shared<Nz::ShaderAst::AssignOp>(op, left, right);
			}
	};

	template<Nz::ShaderAst::BinaryType op>
	class BinOpBuilder
	{
		public:
			std::shared_ptr<Nz::ShaderAst::BinaryOp> operator()(const Nz::ShaderAst::ExpressionPtr& left, const Nz::ShaderAst::ExpressionPtr& right) const
			{
				return std::make_shared<Nz::ShaderAst::BinaryOp>(op, left, right);
			}
	};

	template<Nz::ShaderAst::VariableType type>
	class VarBuilder
	{
		public:
			template<typename... Args>
			std::shared_ptr<Nz::ShaderAst::Variable> operator()(Args&&... args) const
			{
				return std::make_shared<Nz::ShaderAst::NamedVariable>(type, std::forward<Args>(args)...);
			}
	};

	constexpr BinOpBuilder<Nz::ShaderAst::BinaryType::Add> Add;
	constexpr AssignOpBuilder<Nz::ShaderAst::AssignType::Simple> Assign;
	constexpr BinOpBuilder<Nz::ShaderAst::BinaryType::Equality> Equal;
	constexpr GenBuilder<Nz::ShaderAst::StatementBlock> Block;
	constexpr GenBuilder<Nz::ShaderAst::Branch> Branch;
	constexpr GenBuilder<Nz::ShaderAst::Constant> Constant;
	constexpr GenBuilder<Nz::ShaderAst::ExpressionStatement> ExprStatement;
	constexpr VarBuilder<Nz::ShaderAst::VariableType::Parameter> Parameter;
	constexpr VarBuilder<Nz::ShaderAst::VariableType::Uniform> Uniform;
	constexpr VarBuilder<Nz::ShaderAst::VariableType::Variable> Variable;
} }

#include <Nazara/Renderer/ShaderBuilder.inl>

#endif // NAZARA_SHADER_BUILDER_HPP
