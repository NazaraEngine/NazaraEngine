// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Posix/PosixUtils.hpp>
#include <cassert>
#include <cerrno>
#include <utility>
#include <fcntl.h>

namespace Nz::PlatformImpl
{
	int SafeClose(int fd)
	{
#if defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_ANDROID)
		// Retrying close on Linux is dangerous
		// https://android.googlesource.com/platform/bionic/+/master/docs/EINTR.md
		// https://lwn.net/Articles/576478/
		return ::close(fd);
#else
		int ret;
		do
		{
			ret = ::close(fd);
		}
		while (ret == -1 && errno == EINTR);

		return ret;
#endif
	}

	ssize_t SafeRead(int fd, void* buf, size_t count)
	{
		ssize_t ret;
		do
		{
			ret = ::read(fd, buf, count);
		}
		while (ret == -1 && errno == EINTR);

		return ret;
	}

	ssize_t SafeWrite(int fd, const void* buf, size_t count)
	{
		ssize_t ret;
		do
		{
			ret = ::write(fd, buf, count);
		}
		while (ret == -1 && errno == EINTR);

		return ret;
	}

	Pipe::Pipe(int flags) :
	m_readFd(-1),
	m_writeFd(-1)
	{
		int fds[2];
#if defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_ANDROID)
		if (::pipe2(fds, flags | O_CLOEXEC) != 0)
			return;
#else
		if (::pipe(fds) != 0)
			return;

		auto SetCloseOnExec = [](int fd)
		{
			int flags = ::fcntl(fd, F_GETFD, 0);
			if (flags < 0)
				return flags; // error

			return ::fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
		};

		SetCloseOnExec(fds[0]);
		SetCloseOnExec(fds[1]);
#endif

		m_readFd = fds[0];
		m_writeFd = fds[1];
	}

	Pipe::Pipe(Pipe&& other) noexcept :
	m_readFd(std::exchange(other.m_readFd, -1)),
	m_writeFd(std::exchange(other.m_writeFd, -1))
	{
	}

	Pipe::~Pipe()
	{
		if (m_readFd != -1)
		{
			assert(m_writeFd != -1);
			SafeClose(m_readFd);
			SafeClose(m_writeFd);
		}
	}

	Pipe& Pipe::operator=(Pipe&& other) noexcept
	{
		m_readFd = std::exchange(other.m_readFd, -1);
		m_writeFd = std::exchange(other.m_writeFd, -1);

		return *this;
	}

	Pipe::operator bool() const
	{
		return m_readFd != -1;
	}
}
