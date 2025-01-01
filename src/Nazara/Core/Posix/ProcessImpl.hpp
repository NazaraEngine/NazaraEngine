// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_PROCESSIMPL_HPP
#define NAZARA_CORE_POSIX_PROCESSIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Process.hpp>

namespace Nz::PlatformImpl
{
	Result<bool, std::string> CheckProcessExistence(Pid pid);
	Pid GetCurrentProcessId();
	Result<Pid, std::string> SpawnDetachedProcess(const std::filesystem::path& program, std::span<const std::string> arguments = {}, const std::filesystem::path& workingDirectory = {});
}

#endif // NAZARA_CORE_POSIX_PROCESSIMPL_HPP
