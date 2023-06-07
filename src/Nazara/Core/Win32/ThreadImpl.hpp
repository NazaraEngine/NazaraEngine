// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_WIN32_THREADIMPL_HPP
#define NAZARA_CORE_WIN32_THREADIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <string>
#include <Windows.h>

namespace Nz::PlatformImpl
{
	using ThreadHandle = HANDLE;

	HANDLE GetCurrentThreadHandle();
	std::string GetCurrentThreadName();
	std::string GetThreadName(HANDLE threadHandle);
	void RaiseThreadNameException(DWORD threadId, const char* threadName);
	void SetCurrentThreadName(const char* threadName);
	void SetThreadName(HANDLE threadHandle, const char* threadName);
}

#endif // NAZARA_CORE_WIN32_THREADIMPL_HPP
