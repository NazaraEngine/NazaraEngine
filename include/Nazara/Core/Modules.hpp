// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODULES_HPP
#define NAZARA_MODULES_HPP

#include <Nazara/Core/TypeList.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename>
		struct BuildDepList;

		template<typename Module, typename... Modules>
		struct ModuleTuple : ModuleTuple<Module>, ModuleTuple<Modules...>
		{
			template<typename... ModuleConfig>
			ModuleTuple(ModuleConfig&&... configs);
		};

		template<typename Module>
		struct ModuleTuple<Module>
		{
			template<typename... ModuleConfig>
			ModuleTuple(ModuleConfig&&... configs);

			Module m;
		};
	}

	template<typename... ModuleList>
	class Modules
	{
		public:
			template<typename... ModuleConfig>
			Modules(ModuleConfig&&... configs);
			~Modules() = default;

		private:
			using OrderedModuleList = TypeListUnique<typename Detail::BuildDepList<TypeList<ModuleList...>>::Result>;
			using Tuple = TypeListInstantiate<OrderedModuleList, Detail::ModuleTuple>;

			Tuple m_modules;
	};
}

#include <Nazara/Core/Modules.inl>

#endif
