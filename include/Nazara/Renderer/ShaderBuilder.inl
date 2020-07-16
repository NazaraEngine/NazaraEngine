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

	template<ShaderNodes::AssignType op>
	std::shared_ptr<ShaderNodes::AssignOp> AssignOpBuilder<op>::operator()(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right) const
	{
		return ShaderNodes::AssignOp::Build(op, left, right);
	}

	template<ShaderNodes::BinaryType op>
	std::shared_ptr<ShaderNodes::BinaryOp> BinOpBuilder<op>::operator()(const ShaderNodes::ExpressionPtr& left, const ShaderNodes::ExpressionPtr& right) const
	{
		return ShaderNodes::BinaryOp::Build(op, left, right);
	}

	inline std::shared_ptr<ShaderNodes::Variable> BuiltinBuilder::operator()(ShaderNodes::BuiltinEntry builtin) const
	{
		ShaderNodes::BasicType exprType = ShaderNodes::BasicType::Void;

		switch (builtin)
		{
			case ShaderNodes::BuiltinEntry::VertexPosition:
				exprType = ShaderNodes::BasicType::Float4;
				break;
		}

		NazaraAssert(exprType != ShaderNodes::BasicType::Void, "Unhandled builtin");

		return ShaderNodes::BuiltinVariable::Build(builtin, exprType);
	}

	template<ShaderNodes::BasicType Type, typename... Args>
	std::shared_ptr<ShaderNodes::Cast> Cast(Args&&... args)
	{
		return ShaderNodes::Cast::Build(Type, std::forward<Args>(args)...);
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
