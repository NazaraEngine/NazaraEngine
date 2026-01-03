// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Posix/ProcessImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Posix/PosixUtils.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <cerrno>
#include <cstdlib>
#include <signal.h>
#include <spawn.h>
#include <unistd.h>
#include <sys/wait.h>

namespace Nz::PlatformImpl
{
	Result<bool, std::string> CheckProcessExistence(Pid pid)
	{
		if (::kill(pid, 0) == 0)
			return Ok(true);

		if (errno == ESRCH)
			return Ok(false);

		return Err(Error::GetLastSystemError());
	}

	Pid GetCurrentProcessId()
	{
		return ::getpid();
	}

	Result<Pid, std::string> SpawnDetachedProcess(const std::filesystem::path& program, std::span<const std::string> arguments, const std::filesystem::path& workingDirectory)
	{
		struct PidOrErr
		{
			pid_t pid;
			int err;
		};

		Pipe pipe;
		if (!pipe)
			return Err("failed to create pipe: " + Error::GetLastSystemError());

		std::filesystem::path fullpath = std::filesystem::absolute(program);

		// Double fork (see https://0xjet.github.io/3OHA/2022/04/11/post.html)
		// We will create a child and a grand-child process, using a pipe to retrieve the grand-child pid
		pid_t childPid = ::fork();
		if (childPid == -1)
			return Err("failed to create child: " + Error::GetLastSystemError());

		if (childPid == 0)
		{
			// Child process
			::setsid();

			if (!workingDirectory.empty())
			{
				if (::chdir(workingDirectory.c_str()) != 0)
				{
					PidOrErr err;
					err.pid = -1;
					err.err = errno;

					pipe.Write(&err, sizeof(err));

					// Early exit
					_exit(EXIT_FAILURE);
				}
			}

			StackArray<char*> args = NazaraStackArrayNoInit(char*, arguments.size() + 2);

			// It's safe to const_cast here as we're using a copy of the memory (from child) from the original process
			args[0] = const_cast<char*>(program.c_str());
			for (std::size_t i = 0; i < arguments.size(); ++i)
				args[i + 1] = const_cast<char*>(arguments[i].data());

			args.back() = nullptr;

			char* envs[] = { nullptr };

			pid_t grandChildPid;
			if (posix_spawn(&grandChildPid, fullpath.c_str(), nullptr, nullptr, args.data(), envs) == 0)
			{
				PidOrErr pid;
				pid.pid = grandChildPid;

				pipe.Write(&pid, sizeof(pid));

				// Exits the child process, at this point the grand-child should have started
				_exit(EXIT_SUCCESS);
			}
			else
			{
				PidOrErr err;
				err.pid = -1;
				err.err = errno;

				pipe.Write(&err, sizeof(err));

				_exit(EXIT_FAILURE);
			}

			NAZARA_UNREACHABLE();
		}
		else
		{
			// Parent process

			// Wait for and reap the child
			int childStatus;
			::waitpid(childPid, &childStatus, 0);

			PidOrErr pidOrErr;
			if (pipe.Read(&pidOrErr, sizeof(pidOrErr)) != sizeof(pidOrErr))
			{
				// this should never happen
				return Err("failed to create child: couldn't retrieve status from pipe");
			}

			if (pidOrErr.pid < 0)
				return Err(Error::GetLastSystemError(pidOrErr.err));

			return SafeCast<Pid>(pidOrErr.pid);
		}
	}
}
