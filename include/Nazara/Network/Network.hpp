// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_HPP
#define NAZARA_NETWORK_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Network/Config.hpp>

namespace Nz
{
	class NAZARA_NETWORK_API Network : public ModuleBase<Network>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<Core>;

			struct Config {};

			Network(Config /*config*/);
			~Network();

		private:
			static Network* s_instance;
	};
}

#endif // NAZARA_NETWORK_HPP
