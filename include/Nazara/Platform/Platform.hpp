// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PLATFORM_HPP
#define NAZARA_PLATFORM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Platform/Export.hpp>

namespace Nz
{
	class NAZARA_PLATFORM_API Platform : public ModuleBase<Platform>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Platform(Config /*config*/);
			~Platform();

		private:
			static Platform* s_instance;
	};
}

#endif // NAZARA_PLATFORM_HPP
