// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ASSETPARAMETERS_HPP
#define NAZARA_CORE_ASSETPARAMETERS_HPP

#include <Nazara/Core/ParameterList.hpp>

namespace Nz
{
	struct NAZARA_CORE_API AssetParameters
	{
		virtual ~AssetParameters();

		ParameterList custom;
	};
}

#endif // NAZARA_CORE_ASSETPARAMETERS_HPP
