// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_THREADIMPL_HPP
#define NAZARA_CORE_POSIX_THREADIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <string>
#include <pthread.h>

namespace Nz::PlatformImpl
{
	using ThreadHandle = pthread_t;

	pthread_t GetCurrentThreadHandle();
	std::string GetCurrentThreadName();
	std::string GetThreadName(pthread_t threadHandle);
	void SetCurrentThreadName(const char* threadName);
	void SetThreadName(pthread_t threadHandle, const char* threadName);
}

#endif // NAZARA_CORE_POSIX_THREADIMPL_HPP
