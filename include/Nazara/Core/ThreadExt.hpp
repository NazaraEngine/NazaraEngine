// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_THREADEXT_HPP
#define NAZARA_CORE_THREADEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <string>
#include <thread>

namespace Nz
{
	NAZARA_CORE_API std::string GetCurrentThreadName();
	NAZARA_CORE_API std::string GetThreadName(std::thread& thread);

	NAZARA_CORE_API void SetCurrentThreadName(const char* name);
	NAZARA_CORE_API void SetThreadName(std::thread& thread, const char* name);
}

#include <Nazara/Core/ThreadExt.inl>

#endif // NAZARA_CORE_THREADEXT_HPP
