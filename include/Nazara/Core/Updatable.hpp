// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UPDATABLE_HPP
#define NAZARA_UPDATABLE_HPP

#include <Nazara/Prerequesites.hpp>

class NAZARA_API NzUpdatable
{
	public:
		NzUpdatable() = default;
		virtual ~NzUpdatable();

		virtual void Update() = 0;
};

#endif // NAZARA_UPDATABLE_HPP
