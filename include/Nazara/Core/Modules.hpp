// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_MODULES_HPP
#define NAZARA_CORE_MODULES_HPP

#include <Nazara/Core/CommandLineParameters.hpp>
#include <NazaraUtils/TypeList.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename Module, typename... Modules>
		struct ModuleTuple : ModuleTuple<Module>, ModuleTuple<Modules...>
		{
			template<typename... ModuleConfig> ModuleTuple(const CommandLineParameters& parameters, ModuleConfig&&... configs);

			template<typename T> T& Get();
		};

		template<typename Module>
		struct ModuleTuple<Module>
		{
			template<typename... ModuleConfig> ModuleTuple(const CommandLineParameters& parameters, ModuleConfig&&... configs);

			template<typename T> T& Get();

			Module m;
		};
	}

	template<typename> struct OrderedModuleDependencyList;

	template<typename ModuleList> using OrderedModuleDependencies = TypeListUnique<typename OrderedModuleDependencyList<ModuleList>::Result>;

	template<typename... ModuleList>
	class Modules
	{
		public:
			template<typename... ModuleConfig> Modules(ModuleConfig&&... configs);
			template<typename... ModuleConfig> Modules(const CommandLineParameters& parameters, ModuleConfig&&... configs);
			~Modules() = default;

			template<typename T> T& Get();

			using ModuleTypeList = OrderedModuleDependencies<TypeList<ModuleList...>>;

		private:
			using Tuple = TypeListInstantiate<ModuleTypeList, Detail::ModuleTuple>;

			Tuple m_modules;
	};


	template<>
	struct OrderedModuleDependencyList<TypeList<>>
	{
		using Result = TypeList<>;
	};

	template<typename Module, typename... ModuleList>
	struct OrderedModuleDependencyList<TypeList<Module, ModuleList...>>
	{
		using ModuleDependencies = typename OrderedModuleDependencyList<typename Module::Dependencies>::Result;
		using ModuleDependenciesIncModule = TypeListAppend<ModuleDependencies, Module>;
		using RestDependencies = typename OrderedModuleDependencyList<TypeList<ModuleList...>>::Result;
		using Result = TypeListConcat<ModuleDependenciesIncModule, RestDependencies>;
	};
}

#include <Nazara/Core/Modules.inl>

#endif // NAZARA_CORE_MODULES_HPP
