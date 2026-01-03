// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Win32/ProcessImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Win32/Win32Utils.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <Windows.h>

namespace Nz::PlatformImpl
{
	std::wstring BuildCommandLine(const std::filesystem::path& program, std::span<const std::string> arguments)
	{
		std::wstring commandLine;

		auto AddProgramName = [&commandLine](const WidePathHolder& program)
		{
			if (!program.starts_with(L'"') && !program.ends_with(L'"') && program.find(L' ') != program.npos)
			{
				commandLine += L'"';
				commandLine += program;
				commandLine += L'"';
			}
			else
				commandLine += program;

			ReplaceStr(commandLine, L'/', L'\\');
			commandLine += L' ';
		};

		// Use a lambda to keep WidePathHolder alive
		AddProgramName(PathToWideTemp(program));

		// https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
		for (std::string_view arg : arguments)
		{
			commandLine += L' ';

			if (arg.empty())
			{
				// Empty argument (ensures quotes)
				commandLine += LR"("")";
				continue;
			}

			// Characters requiring quotes from cmd /?
			constexpr std::string_view specialChars = "\t &()[]{}^=;!'+,`~%|<>";

			bool requiresQuote = arg.find_first_of(specialChars) != arg.npos;

			if (requiresQuote)
				commandLine += L'"';

			std::size_t backslashCount = 0;
			IterateOnWideChars(arg, [&](std::wstring_view characters)
			{
				for (wchar_t character : characters)
				{
					if (character != L'\\')
					{
						// Escape quotes and double their preceding backslashes ('\\\"' => '\\\\\\\"')
						if (character == L'"')
							commandLine.append(backslashCount + 1, L'\\');

						backslashCount = 0;
					}
					else
						backslashCount++;

					commandLine.push_back(character);
				}

				return true;
			});

			if (requiresQuote)
			{
				commandLine.append(backslashCount, L'\\');
				commandLine += L'"';
			}
		}

		return commandLine;
	}

	Result<bool, std::string> CheckProcessExistence(Pid pid)
	{
		HANDLE processHandle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
		if (!processHandle)
		{
			DWORD lastErr = ::GetLastError();
			if (lastErr == ERROR_INVALID_PARAMETER)
				return Ok(false);

			return Err("failed to open process handle: " + Error::GetLastSystemError());
		}
		CallOnExit releaseHandle([processHandle] { ::CloseHandle(processHandle); });

		// Opening a handle doesn't mean the process hasn't exited (zombie processes), check if it has an exit code
		DWORD exitCode;
		if (!::GetExitCodeProcess(processHandle, &exitCode))
			return Err("failed to get exit code: " + Error::GetLastSystemError());

		return Ok(exitCode == STILL_ACTIVE);
	}

	Pid GetCurrentProcessId()
	{
		return ::GetCurrentProcessId();
	}

	Result<Pid, std::string> SpawnDetachedProcess(const std::filesystem::path& program, std::span<const std::string> arguments, const std::filesystem::path& workingDirectory)
	{
		DWORD creationFlags = CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;

		std::wstring commandLine = BuildCommandLine(program, arguments);

		STARTUPINFOW startupInfo = {
			.cb = sizeof(startupInfo),
			.dwX = DWORD(CW_USEDEFAULT),
			.dwY = DWORD(CW_USEDEFAULT),
			.dwXSize = DWORD(CW_USEDEFAULT),
			.dwYSize = DWORD(CW_USEDEFAULT)
		};

		PROCESS_INFORMATION processInfo;
		BOOL success = ::CreateProcessW(
			nullptr,            // Application name
			commandLine.data(), // Command line
			nullptr,            // Process attributes
			nullptr,            // Thread attributes
			false,              // Inherit handles
			creationFlags,      // Creation flags
			nullptr,            // Environment
			(!workingDirectory.empty()) ? PathToWideTemp(workingDirectory).data() : nullptr, // Current directory
			&startupInfo,       // Startup info
			&processInfo        // Process information
		);

		if (!success)
			return Err(Error::GetLastSystemError());

		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return Pid(processInfo.dwProcessId);
	}
}

#include <Nazara/Core/AntiWindows.hpp>
