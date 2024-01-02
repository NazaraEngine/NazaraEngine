// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONCOMPONENTREGISTRY_HPP
#define NAZARA_CORE_APPLICATIONCOMPONENTREGISTRY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>

namespace Nz
{
	template<typename T>
	struct ApplicationComponentRegistry
	{
		static std::size_t GetComponentId();
	};
}

#include <Nazara/Core/ApplicationComponentRegistry.inl>

#endif // NAZARA_CORE_APPLICATIONCOMPONENTREGISTRY_HPP
