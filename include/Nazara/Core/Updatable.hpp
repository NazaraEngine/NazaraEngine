// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_UPDATABLE_HPP
#define NAZARA_CORE_UPDATABLE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>

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

#endif // NAZARA_CORE_UPDATABLE_HPP
