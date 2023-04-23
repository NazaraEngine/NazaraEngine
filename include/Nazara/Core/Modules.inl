// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
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
				{
					NazaraUnused(first);
					return Get(std::forward<Args>(args)...);
				}
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
		ModuleTuple<Module>::ModuleTuple(ModuleConfig&&... configs) :
		m(Pick<typename Module::Config>::Get(std::forward<ModuleConfig>(configs)...))
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
	m_modules(std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename T>
	T& Modules<ModuleList...>::Get()
	{
		return m_modules.template Get<T>();
	}
}

#include <Nazara/Core/DebugOff.hpp>
