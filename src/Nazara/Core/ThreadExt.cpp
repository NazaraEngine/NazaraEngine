// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ThreadExt.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/Win32/ThreadImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Core/Posix/ThreadImpl.hpp>
#else
#error OS not handled
#endif


namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		PlatformImpl::ThreadHandle GetHandle(std::thread& thread)
		{
#ifdef NAZARA_COMPILER_MINGW_THREADS_MCF
			// MCF flavor (returns HANDLE by a void*)
			return static_cast<PlatformImpl::ThreadHandle>(_MCF_thread_get_handle(thread.native_handle()));
#else
			// Cast because of MSVC standard library that returns a void* instead of a HANDLE
			return static_cast<PlatformImpl::ThreadHandle>(thread.native_handle());
#endif
		}
	}

	std::string GetCurrentThreadName()
	{
		return PlatformImpl::GetCurrentThreadName();
	}

	std::string GetThreadName(std::thread& thread)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return PlatformImpl::GetThreadName(GetHandle(thread));
	}

	void SetCurrentThreadName(const char* name)
	{
		PlatformImpl::SetCurrentThreadName(name);
	}

	void SetThreadName(std::thread& thread, const char* name)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		PlatformImpl::SetThreadName(GetHandle(thread), name);
	}
}
