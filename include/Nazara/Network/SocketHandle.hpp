// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOCKETHANDLE_HPP
#define NAZARA_SOCKETHANDLE_HPP

#include <Nazara/Prerequisites.hpp>

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

#endif // NAZARA_SOCKETHANDLE_HPP
