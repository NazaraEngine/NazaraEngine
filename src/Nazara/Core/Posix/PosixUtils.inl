// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz::PlatformImpl
{
	inline int Pipe::GetReadFd()
	{
		return m_readFd;
	}

	inline int Pipe::GetWriteFd()
	{
		return m_writeFd;
	}

	inline ssize_t Pipe::Read(void* buf, size_t count)
	{
		return SafeRead(m_readFd, buf, count);
	}

	inline ssize_t Pipe::Write(const void* buf, size_t count)
	{
		return SafeWrite(m_writeFd, buf, count);
	}
}
