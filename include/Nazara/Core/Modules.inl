// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Modules.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz::Detail
{
	template<>
	struct BuildDepList<TypeList<>>
	{
		using Result = TypeList<>;
	};

	template<typename Module, typename... ModuleList>
	struct BuildDepList<TypeList<Module, ModuleList...>>
	{
		using ModuleDependencies = typename BuildDepList<typename Module::Dependencies>::Result;
		using ModuleDependenciesIncModule = TypeListAppend<ModuleDependencies, Module>;
		using RestDependencies = typename BuildDepList<TypeList<ModuleList...>>::Result;
		using Result = TypeListConcat<ModuleDependenciesIncModule, RestDependencies>;
	};
}

#include <Nazara/Core/DebugOff.hpp>
