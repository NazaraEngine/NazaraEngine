// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/Module.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline Module::Module(UInt32 shaderLangVersion, const Uuid& uuid)
	{
		auto mutMetadata = std::make_shared<Metadata>();
		mutMetadata->moduleId = uuid;
		mutMetadata->shaderLangVersion = shaderLangVersion;

		metadata = std::move(mutMetadata);
		rootNode = ShaderBuilder::MultiStatement();
	}

	inline Module::Module(std::shared_ptr<const Metadata> metadata, std::vector<ImportedModule> importedModules) :
	Module(std::move(metadata), ShaderBuilder::MultiStatement(), std::move(importedModules))
	{
	}

	inline Module::Module(std::shared_ptr<const Metadata> Metadata, MultiStatementPtr RootNode, std::vector<ImportedModule> ImportedModules) :
	metadata(std::move(Metadata)),
	importedModules(std::move(ImportedModules)),
	rootNode(std::move(RootNode))
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
