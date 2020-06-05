// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz::ShaderBuilder
{
	template<typename T>
	template<typename... Args>
	std::shared_ptr<T> GenBuilder<T>::operator()(Args&&... args) const
	{
		return T::Build(std::forward<Args>(args)...);
	}

	template<ShaderAst::AssignType op>
	std::shared_ptr<ShaderAst::AssignOp> AssignOpBuilder<op>::operator()(const ShaderAst::ExpressionPtr& left, const ShaderAst::ExpressionPtr& right) const
	{
		return ShaderAst::AssignOp::Build(op, left, right);
	}

	template<ShaderAst::BinaryType op>
	std::shared_ptr<ShaderAst::BinaryOp> BinOpBuilder<op>::operator()(const ShaderAst::ExpressionPtr& left, const ShaderAst::ExpressionPtr& right) const
	{
		return ShaderAst::BinaryOp::Build(op, left, right);
	}

	inline std::shared_ptr<ShaderAst::Variable> BuiltinBuilder::operator()(ShaderAst::BuiltinEntry builtin) const
	{
		ShaderAst::ExpressionType exprType = ShaderAst::ExpressionType::Void;

		switch (builtin)
		{
			case ShaderAst::BuiltinEntry::VertexPosition:
				exprType = ShaderAst::ExpressionType::Float4;
				break;
		}

		NazaraAssert(exprType != ShaderAst::ExpressionType::Void, "Unhandled builtin");

		return ShaderAst::BuiltinVariable::Build(builtin, exprType);
	}

	template<ShaderAst::VariableType type>
	template<typename... Args>
	ShaderAst::NamedVariablePtr VarBuilder<type>::operator()(Args&&... args) const
	{
		return ShaderAst::NamedVariable::Build(type, std::forward<Args>(args)...);
	}

	template<ShaderAst::ExpressionType Type, typename... Args>
	std::shared_ptr<ShaderAst::Cast> Cast(Args&&... args)
	{
		return ShaderAst::Cast::Build(Type, std::forward<Args>(args)...);
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
