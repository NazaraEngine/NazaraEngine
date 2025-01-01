// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_SOCKETHANDLE_HPP
#define NAZARA_NETWORK_SOCKETHANDLE_HPP

#include <NazaraUtils/Prerequisites.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <basetsd.h>
#endif

namespace Nz
{
	#if defined(NAZARA_PLATFORM_WINDOWS)
	using SocketHandle = UINT_PTR;
	#elif defined(NAZARA_PLATFORM_POSIX)
	using SocketHandle = int;
	#else
		#error Lack of implementation: SocketHandle
	#endif
}

#endif // NAZARA_NETWORK_SOCKETHANDLE_HPP
