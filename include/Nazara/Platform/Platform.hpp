// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_HPP
#define NAZARA_PLATFORM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Utility/Utility.hpp>

namespace Nz
{
	class NAZARA_PLATFORM_API Platform : public ModuleBase<Platform>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Utility>;

			struct Config {};

			Platform(Config /*config*/);
			~Platform();

		private:
			static Platform* s_instance;
	};
}

#endif // NAZARA_PLATFORM_HPP
