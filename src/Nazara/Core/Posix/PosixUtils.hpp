// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_POSIXUTILS_HPP
#define NAZARA_CORE_POSIX_POSIXUTILS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <unistd.h>

namespace Nz::PlatformImpl
{
	int SafeClose(int fd);
	ssize_t SafeRead(int fd, void* buf, size_t count);
	ssize_t SafeWrite(int fd, const void* buf, size_t count);

	class Pipe
	{
		public:
			Pipe(int flags = 0);
			Pipe(const Pipe&) = delete;
			Pipe(Pipe&& other) noexcept;
			~Pipe();

			inline int GetReadFd();
			inline int GetWriteFd();

			inline ssize_t Read(void* buf, size_t count);
			inline ssize_t Write(const void* buf, size_t count);

			Pipe& operator=(const Pipe&) = delete;
			Pipe& operator=(Pipe&& other) noexcept;

			explicit operator bool() const;

		private:
			int m_readFd;
			int m_writeFd;
	};
}

#include <Nazara/Core/Posix/PosixUtils.inl>

#endif // NAZARA_CORE_POSIX_POSIXUTILS_HPP
