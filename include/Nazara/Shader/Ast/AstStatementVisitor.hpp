// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTSTATEMENTVISITOR_HPP
#define NAZARA_SHADER_AST_ASTSTATEMENTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API AstStatementVisitor
	{
		public:
			AstStatementVisitor() = default;
			AstStatementVisitor(const AstStatementVisitor&) = delete;
			AstStatementVisitor(AstStatementVisitor&&) = delete;
			virtual ~AstStatementVisitor();

#define NAZARA_SHADERAST_STATEMENT(NodeType) virtual void Visit(ShaderAst::NodeType& node) = 0;
#include <Nazara/Shader/Ast/AstNodeList.hpp>

			AstStatementVisitor& operator=(const AstStatementVisitor&) = delete;
			AstStatementVisitor& operator=(AstStatementVisitor&&) = delete;
	};
}

#endif // NAZARA_SHADER_AST_ASTSTATEMENTVISITOR_HPP
