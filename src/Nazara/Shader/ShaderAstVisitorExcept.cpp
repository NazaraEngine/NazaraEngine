// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/ShaderAstVisitorExcept.hpp>
#include <stdexcept>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	void ShaderAstVisitorExcept::Visit(ShaderNodes::AccessMember& /*node*/)
	{
		throw std::runtime_error("unhandled AccessMember node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::AssignOp& /*node*/)
	{
		throw std::runtime_error("unhandled AssignOp node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::BinaryOp& /*node*/)
	{
		throw std::runtime_error("unhandled AccessMember node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::Branch& /*node*/)
	{
		throw std::runtime_error("unhandled Branch node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::Cast& /*node*/)
	{
		throw std::runtime_error("unhandled Cast node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::Constant& /*node*/)
	{
		throw std::runtime_error("unhandled Constant node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::DeclareVariable& /*node*/)
	{
		throw std::runtime_error("unhandled DeclareVariable node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::ExpressionStatement& /*node*/)
	{
		throw std::runtime_error("unhandled ExpressionStatement node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::Identifier& /*node*/)
	{
		throw std::runtime_error("unhandled Identifier node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::IntrinsicCall& /*node*/)
	{
		throw std::runtime_error("unhandled IntrinsicCall node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::Sample2D& /*node*/)
	{
		throw std::runtime_error("unhandled Sample2D node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::StatementBlock& /*node*/)
	{
		throw std::runtime_error("unhandled StatementBlock node");
	}

	void ShaderAstVisitorExcept::Visit(ShaderNodes::SwizzleOp& /*node*/)
	{
		throw std::runtime_error("unhandled SwizzleOp node");
	}
}
