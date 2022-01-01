// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HPP
#define NAZARA_CORE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ModuleBase.hpp>
#include <Nazara/Core/Modules.hpp>
#include <Nazara/Core/TypeList.hpp>

namespace Nz
{
	class NAZARA_CORE_API Core : public ModuleBase<Core>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<>;

			struct Config {};

			Core(Config /*config*/);
			~Core();

		private:
			static Core* s_instance;
	};
}

#include <Nazara/Core/Core.inl>

#endif // NAZARA_CORE_HPP
