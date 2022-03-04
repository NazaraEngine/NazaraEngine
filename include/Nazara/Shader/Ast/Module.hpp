// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_MODULE_HPP
#define NAZARA_SHADER_AST_MODULE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <memory>

namespace Nz::ShaderAst
{
	struct Module;

	using ModulePtr = std::shared_ptr<Module>;

	struct Module
	{
		struct Metadata
		{
			UInt32 shaderLangVersion;
		};

		std::shared_ptr<const Metadata> metadata;
		MultiStatementPtr rootNode;
	};
}

#endif // NAZARA_SHADER_AST_MODULE_HPP
