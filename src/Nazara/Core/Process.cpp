// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Process.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/Win32/ProcessImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Core/Posix/ProcessImpl.hpp>
#else
#error OS not handled
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Result<bool, std::string> Process::Exists(Pid pid)
	{
		return PlatformImpl::CheckProcessExistence(pid);
	}

	Pid Process::GetCurrentPid()
	{
		return PlatformImpl::GetCurrentProcessId();
	}

	Result<Pid, std::string> Process::SpawnDetached(const std::filesystem::path& program, std::span<const std::string> arguments, const std::filesystem::path& workingDirectory)
	{
		return PlatformImpl::SpawnDetachedProcess(program, arguments, workingDirectory);
	}
}
