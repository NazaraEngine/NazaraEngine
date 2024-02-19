// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <utility>

namespace Nz
{
	namespace Detail
	{
		template<typename, typename = void>
		struct ModuleConfigHasOverride : std::false_type {};

		template<typename T>
		struct ModuleConfigHasOverride<T, std::void_t<decltype(std::declval<T>().Override(std::declval<const CommandLineParameters&>()))>> : std::true_type {};

		template<typename T>
		decltype(auto) OverrideModuleConfig(T&& module, const CommandLineParameters& params)
		{
			if constexpr (!std::is_const_v<T> && ModuleConfigHasOverride<T>::value)
				module.Override(params);

			return std::forward<T>(module);
		}

		template<typename T>
		struct Pick
		{
			template<typename First, typename... Args>
			static decltype(auto) Get(First&& first, Args&&... args)
			{
				if constexpr (std::is_same_v<T, std::decay_t<First>>)
					return std::forward<First>(first);
				else
				{
					NazaraUnused(first);
					return Get(std::forward<Args>(args)...);
				}
			}

			template<typename First, typename... Args>
			static decltype(auto) Get(const CommandLineParameters& parameters, First&& first, Args&&... args)
			{
				if constexpr (std::is_same_v<T, std::decay_t<First>>)
					return OverrideModuleConfig<First>(first, parameters);
				else
				{
					NazaraUnused(first);
					return Get(parameters, std::forward<Args>(args)...);
				}
			}

			static auto Get(const CommandLineParameters& parameters)
			{
				return OverrideModuleConfig(T{}, parameters);
			}
		};

		template<typename Module, typename... Modules>
		template<typename... ModuleConfig>
		ModuleTuple<Module, Modules...>::ModuleTuple(const CommandLineParameters& parameters, ModuleConfig&&... configs) :
		ModuleTuple<Module>(parameters, std::forward<ModuleConfig>(configs)...),
		ModuleTuple<Modules...>(parameters, std::forward<ModuleConfig>(configs)...)
		{
		}

		template<typename Module, typename... Modules>
		template<typename T>
		T& ModuleTuple<Module, Modules...>::Get()
		{
			if constexpr (std::is_same_v<T, Module>)
				return ModuleTuple<Module>::template Get<T>();
			else
				return ModuleTuple<Modules...>::template Get<T>();
		}

		template<typename Module>
		template<typename... ModuleConfig>
		ModuleTuple<Module>::ModuleTuple(const CommandLineParameters& parameters, ModuleConfig&&... configs) :
		m(Pick<typename Module::Config>::Get(parameters, std::forward<ModuleConfig>(configs)...))
		{
		}

		template<typename Module>
		template<typename T>
		T& ModuleTuple<Module>::Get()
		{
			static_assert(std::is_same_v<T, Module>, "module is not in the list");
			return m;
		}
	}

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Modules<ModuleList...>::Modules(ModuleConfig&&... configs) :
	m_modules(CommandLineParameters{}, std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Modules<ModuleList...>::Modules(const CommandLineParameters& parameters, ModuleConfig&&... configs) :
	m_modules(parameters, std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename T>
	T& Modules<ModuleList...>::Get()
	{
		return m_modules.template Get<T>();
	}
}

