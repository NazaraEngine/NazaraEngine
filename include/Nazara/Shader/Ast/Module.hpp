// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SHADER_AST_MODULE_HPP
#define NAZARA_SHADER_AST_MODULE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Uuid.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Ast/Nodes.hpp>
#include <memory>

namespace Nz::ShaderAst
{
	class Module;

	using ModulePtr = std::shared_ptr<Module>;

	class Module
	{
		public:
			struct Metadata;

			inline Module(UInt32 shaderLangVersion, const Uuid& moduleId = Uuid::Generate());
			inline Module(std::shared_ptr<const Metadata> metadata);
			inline Module(std::shared_ptr<const Metadata> metadata, MultiStatementPtr rootNode);
			Module(const Module&) = default;
			Module(Module&&) noexcept = default;
			~Module() = default;

			Module& operator=(const Module&) = default;
			Module& operator=(Module&&) noexcept = default;

			struct ImportedModule
			{
				std::string identifier;
				std::vector<Uuid> dependencies;
				ModulePtr module;
			};

			struct Metadata
			{
				UInt32 shaderLangVersion;
				Uuid moduleId;
			};

			std::shared_ptr<const Metadata> metadata;
			std::vector<ImportedModule> importedModules;
			MultiStatementPtr rootNode;
	};
}

#include <Nazara/Shader/Ast/Module.inl>

#endif // NAZARA_SHADER_AST_MODULE_HPP
