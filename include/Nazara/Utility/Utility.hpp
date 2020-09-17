// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_HPP
#define NAZARA_UTILITY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API Utility : public ModuleBase<Utility>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Utility(Config /*config*/);
			~Utility();

		private:
			static Utility* s_instance;
	};
}

#endif // NAZARA_UTILITY_HPP
