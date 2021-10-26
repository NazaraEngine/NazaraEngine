// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_TYPETAG_HPP
#define NAZARA_CORE_TYPETAG_HPP

namespace Nz
{
	template<typename T>
	struct TypeTag
	{
		using Type = T;
	};
}

#endif // NAZARA_CORE_TYPETAG_HPP
