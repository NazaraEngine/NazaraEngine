// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	namespace Detail
	{
		template<typename, typename, typename = void>
		struct ModuleHasRegister : std::false_type {};

		template<typename M, typename C>
		struct ModuleHasRegister<M, C, std::void_t<decltype(std::declval<M>().RegisterComponent(std::declval<C&>()))>> : std::true_type {};

		template<typename> struct ModuleRegisterer;

		template<typename Module, typename... Rest>
		struct ModuleRegisterer<TypeList<Module, Rest...>>
		{
			template<typename T, typename C>
			static void Register(T& modules, C& component)
			{
				if constexpr (ModuleHasRegister<Module, C>::value)
					modules.template Get<Module>().RegisterComponent(component);

				if constexpr (sizeof...(Rest) > 0)
					ModuleRegisterer<TypeList<Rest...>>::Register(modules, component);
			}
		};
	}

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Application<ModuleList...>::Application(ModuleConfig&&... configs) :
	m_modules(std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Application<ModuleList...>::Application(int argc, char** argv, ModuleConfig&&... configs) :
	ApplicationBase(argc, argv),
	m_modules(GetCommandLineParameters(), std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename... ModuleConfig>
	Application<ModuleList...>::Application(int argc, const Pointer<const char>* argv, ModuleConfig&&... configs) :
	ApplicationBase(argc, argv),
	m_modules(GetCommandLineParameters(), std::forward<ModuleConfig>(configs)...)
	{
	}

	template<typename... ModuleList>
	template<typename T, typename... Args>
	T& Application<ModuleList...>::AddComponent(Args&&... args)
	{
		T& component = ApplicationBase::AddComponent<T>(std::forward<Args>(args)...);
		Detail::ModuleRegisterer<typename decltype(m_modules)::ModuleTypeList>::template Register(m_modules, component);

		return component;
	}

	template<typename... ModuleList>
	Application<ModuleList...>::~Application()
	{
		// Clear components before releasing modules
		ClearComponents();
	}
}

