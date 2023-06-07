// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/ThreadImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <array>
#include <Nazara/Core/Debug.hpp>

// https://stackoverflow.com/questions/2369738/how-to-set-the-name-of-a-thread-in-linux-pthreads

namespace Nz::PlatformImpl
{
	pthread_t GetCurrentThreadHandle()
	{
		return ::pthread_self();
	}

	std::string GetCurrentThreadName()
	{
		return GetThreadName(::pthread_self());
	}

	std::string GetThreadName(pthread_t threadHandle)
	{
#if defined(__linux__) || defined(__APPLE__)
		std::array<char, 16> name;
		::pthread_getname_np(threadHandle, &name[0], name.size());

		return std::string(&name[0]);
#else
		return "<unsupported>";
#endif
	}

	void SetCurrentThreadName(const char* threadName)
	{
#if defined(__linux__) || defined(NAZARA_COMPILER_MINGW)
		::pthread_setname_np(::pthread_self(), threadName);
#elif defined(__APPLE__)
		::pthread_setname_np(threadName);
#elif defined(__FreeBSD__)
		::pthread_set_name_np(::pthread_self(), threadName);
#else
		NazaraWarning("setting current thread name is not supported on this platform");
#endif
	}

	void SetThreadName(pthread_t threadHandle, const char* threadName)
	{
#if defined(__linux__) || defined(NAZARA_COMPILER_MINGW)
		::pthread_setname_np(threadHandle, threadName);
#elif defined(__APPLE__)
		NazaraWarning("only current thread name can be set on MacOS X");
#elif defined(__FreeBSD__)
		::pthread_set_name_np(threadHandle, threadName);
#else
		NazaraWarning("setting a thread name is not supported on this platform");
#endif
	}
}

