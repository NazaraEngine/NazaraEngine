// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_RESOURCEPARAMETERS_HPP
#define NAZARA_CORE_RESOURCEPARAMETERS_HPP

#include <Nazara/Core/ParameterList.hpp>

namespace Nz
{
	struct NAZARA_CORE_API ResourceParameters
	{
		virtual ~ResourceParameters();

		ParameterList custom;
	};
}

#endif // NAZARA_CORE_RESOURCEPARAMETERS_HPP
