// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz { namespace ShaderBuilder
{
	template<typename T>
	template<typename... Args>
	std::shared_ptr<T> GenBuilder<T>::operator()(Args&&... args) const
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<ShaderAst::AssignType op>
	std::shared_ptr<ShaderAst::AssignOp> AssignOpBuilder<op>::operator()(const ShaderAst::VariablePtr& left, const ShaderAst::ExpressionPtr& right) const
	{
		return std::make_shared<ShaderAst::AssignOp>(op, left, right);
	}

	template<ShaderAst::BinaryType op>
	std::shared_ptr<ShaderAst::BinaryOp> BinOpBuilder<op>::operator()(const ShaderAst::ExpressionPtr& left, const ShaderAst::ExpressionPtr& right) const
	{
		return std::make_shared<ShaderAst::BinaryOp>(op, left, right);
	}

	std::shared_ptr<ShaderAst::Variable> BuiltinBuilder::operator()(ShaderAst::Builtin builtin) const
	{
		ShaderAst::ExpressionType exprType = ShaderAst::ExpressionType::None;

		switch (builtin)
		{
			case ShaderAst::Builtin::VertexPosition:
				exprType = ShaderAst::ExpressionType::Float4;
				break;
		}

		NazaraAssert(exprType != ShaderAst::ExpressionType::None, "Unhandled builtin");

		return std::make_shared<ShaderAst::BuiltinVariable>(builtin, exprType);
	}

	template<ShaderAst::VariableType type>
	template<typename... Args>
	std::shared_ptr<ShaderAst::Variable> VarBuilder<type>::operator()(Args&&... args) const
	{
		return std::make_shared<ShaderAst::NamedVariable>(type, std::forward<Args>(args)...);
	}

	template<ShaderAst::ExpressionType Type, typename... Args>
	std::shared_ptr<ShaderAst::Cast> Cast(Args&&... args)
	{
		return std::make_shared<ShaderAst::Cast>(Type, std::forward<Args>(args)...);
	}
} }

#include <Nazara/Renderer/DebugOff.hpp>
