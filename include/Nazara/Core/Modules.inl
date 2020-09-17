// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Modules.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename T>
		struct Pick
		{
			template<typename First, typename... Args>
			static auto Get(First&& first, Args&&... args)
			{
				if constexpr (std::is_same_v<T, std::decay_t<First>>)
					return std::forward<First>(first);
				else
					return Get(std::forward<Args>(args)...);
			}

			static auto Get()
			{
				return T{};
			}
		};

		template<typename Module, typename... Modules>
		template<typename... ModuleConfig>
		ModuleTuple<Module, Modules...>::ModuleTuple(ModuleConfig&&... configs) :
		ModuleTuple<Module>(std::forward<ModuleConfig>(configs)...),
		ModuleTuple<Modules...>(std::forward<ModuleConfig>(configs)...)
		{
		}

		template<typename Module>
		template<typename... ModuleConfig>
		ModuleTuple<Module>::ModuleTuple(ModuleConfig&&... configs) :
		m(Pick<typename Module::Config>::Get(std::forward<ModuleConfig>(configs)...))
		{
		}

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

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Modules<ModuleList...>::Modules(ModuleConfig&&... configs) :
	m_modules(std::forward<ModuleConfig>(configs)...)
	{
	}
}

#include <Nazara/Core/DebugOff.hpp>
