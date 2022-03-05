// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Ast/Module.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz::ShaderAst
{
	inline Module::Module(UInt32 shaderLangVersion)
	{
		auto mutMetadata = std::make_shared<Metadata>();
		mutMetadata->moduleId = Uuid::Generate();
		mutMetadata->shaderLangVersion = shaderLangVersion;

		metadata = std::move(mutMetadata);
		rootNode = ShaderBuilder::MultiStatement();
	}

	inline Module::Module(std::shared_ptr<const Metadata> metadata) :
	Module(std::move(metadata), ShaderBuilder::MultiStatement())
	{
	}

	inline Module::Module(std::shared_ptr<const Metadata> Metadata, MultiStatementPtr RootNode) :
	metadata(std::move(Metadata)),
	rootNode(std::move(RootNode))
	{
	}
}

#include <Nazara/Shader/DebugOff.hpp>
