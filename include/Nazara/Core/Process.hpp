// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PROCESS_HPP
#define NAZARA_CORE_PROCESS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/Result.hpp>
#include <filesystem>
#include <span>
#include <string>

namespace Nz
{
	using Pid = UInt32;

	class NAZARA_CORE_API Process
	{
		public:
			Process() = default;
			Process(const Process&) = delete;
			Process(Process&&) = delete;
			~Process() = default;

			Process& operator=(const Process&) = delete;
			Process& operator=(Process&&) = delete;

			static Result<bool, std::string> Exists(Pid pid);
			static Pid GetCurrentPid();
			static Result<Pid, std::string> SpawnDetached(const std::filesystem::path& program, std::span<const std::string> arguments = {}, const std::filesystem::path& workingDirectory = {});
	};
}

#include <Nazara/Core/Process.inl>

#endif // NAZARA_CORE_PROCESS_HPP
