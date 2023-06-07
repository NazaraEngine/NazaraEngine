// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ThreadExt.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/Win32/ThreadImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Core/Posix/ThreadImpl.hpp>
#else
#error OS not handled
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	std::string GetCurrentThreadName()
	{
		return PlatformImpl::GetCurrentThreadName();
	}

	std::string GetThreadName(std::thread& thread)
	{
		// std::thread::native_handle returns a void* with MSVC instead of a HANDLE
		return PlatformImpl::GetThreadName(static_cast<PlatformImpl::ThreadHandle>(thread.native_handle()));
	}

	void SetCurrentThreadName(const char* name)
	{
		PlatformImpl::SetCurrentThreadName(name);
	}

	void SetThreadName(std::thread& thread, const char* name)
	{
		// std::thread::native_handle returns a void* with MSVC instead of a HANDLE
		PlatformImpl::SetThreadName(static_cast<PlatformImpl::ThreadHandle>(thread.native_handle()), name);
	}
}
