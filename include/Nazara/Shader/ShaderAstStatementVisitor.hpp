// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTSTATEMENTVISITOR_HPP
#define NAZARA_SHADERASTSTATEMENTVISITOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderNodes.hpp>

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
#include <Nazara/Shader/ShaderAstNodes.hpp>

			AstStatementVisitor& operator=(const AstStatementVisitor&) = delete;
			AstStatementVisitor& operator=(AstStatementVisitor&&) = delete;
	};
}

#endif
