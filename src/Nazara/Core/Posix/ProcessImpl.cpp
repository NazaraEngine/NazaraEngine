// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/ProcessImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Posix/PosixUtils.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz::PlatformImpl
{
	Result<bool, std::string> CheckProcessExistence(Pid pid)
	{
		if (::kill(pid, 0) == 0)
			return Ok(true);

		if (errno == ESRCH)
			return OK(false);

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

		// Double fork (see https://0xjet.github.io/3OHA/2022/04/11/post.html)
		// We will create a child and a grand-child process, using a pipe to retrieve the grand-child pid
		// TODO: Maybe use vfork for the child process too?
		// TODO: Use posix_spawn if possible instead
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
					std::exit(1);
				}
			}

			pid_t grandChildPid = ::vfork();
			if (grandChildPid == 0)
			{
				// Grand-child process
				StackArray<char*> argv = NazaraStackArrayNoInit(char*, arguments.size() + 2);

				// It's safe to const_cast here as we're using a copy of the memory (from child) from the original process
				argv[0] = const_cast<char*>(program.c_str());
				for (std::size_t i = 0; i < arguments.size(); ++i)
					argv[i + 1] = const_cast<char*>(arguments[i].data());

				argv[argv.size() - 1] = nullptr;

				char* envs[] = { nullptr };

				::execve(program.c_str(), argv.data(), envs);

				// If we get here, execve failed
				// Remember we share the memory of our parent (vfork) so we need to exit using _exit() to avoid calling the parent exit handlers

				PidOrErr err;
				err.pid = -1;
				err.err = errno;

				pipe.Write(&err, sizeof(err));

				_exit(1);
			}
			else if (grandChildPid != -1)
			{
				PidOrErr err;
				err.pid = grandChildPid;

				pipe.Write(&err, sizeof(err));

				// Exits the child process, at this point the grand-child should have started
				std::exit(0);
			}
			else
			{
				PidOrErr err;
				err.pid = -1;
				err.err = errno;

				pipe.Write(&err, sizeof(err));

				std::exit(1);
			}

			__builtin_unreachable(); // TODO: NAZARA_UNREACHABLE()
		}
		else
		{
			// Parent process

			// Wait for and reap the child
			int childStatus;
			::waitpid(childPid, &childStatus, 0);

			PidOrErr pidOrErr;
			if (pipe.Read(&pidOrErr, sizeof(pidOrErr) != sizeof(pidOrErr)))
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
