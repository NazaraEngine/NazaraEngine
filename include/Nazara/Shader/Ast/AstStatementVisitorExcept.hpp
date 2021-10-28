// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_ASTSTATEMENTVISITOREXCEPT_HPP
#define NAZARA_SHADER_AST_ASTSTATEMENTVISITOREXCEPT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/AstStatementVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API StatementVisitorExcept : public AstStatementVisitor
	{
		public:
			using AstStatementVisitor::Visit;

#define NAZARA_SHADERAST_STATEMENT(Node) void Visit(ShaderAst::Node& node) override;
#include <Nazara/Shader/Ast/AstNodeList.hpp>
	};
}

#endif // NAZARA_SHADER_AST_ASTSTATEMENTVISITOREXCEPT_HPP
