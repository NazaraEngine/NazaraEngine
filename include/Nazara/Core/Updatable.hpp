// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UPDATABLE_HPP
#define NAZARA_UPDATABLE_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class NAZARA_CORE_API Updatable
	{
		public:
			Updatable() = default;
			virtual ~Updatable();

			virtual void Update() = 0;
	};
}

#endif // NAZARA_UPDATABLE_HPP
