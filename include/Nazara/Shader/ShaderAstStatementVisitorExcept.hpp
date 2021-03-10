// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADERASTSTATEMENTVISITOREXCEPT_HPP
#define NAZARA_SHADERASTSTATEMENTVISITOREXCEPT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/ShaderAstStatementVisitor.hpp>

namespace Nz::ShaderAst
{
	class NAZARA_SHADER_API StatementVisitorExcept : public AstStatementVisitor
	{
		public:
			using AstStatementVisitor::Visit;

#define NAZARA_SHADERAST_STATEMENT(Node) void Visit(ShaderAst::Node& node) override;
#include <Nazara/Shader/ShaderAstNodes.hpp>
	};
}

#endif
