// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#ifdef NAZARA_PLATFORM_WINDOWS
#include <WS2tcpip.h>
#include <WinSock2.h>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <sys/socket.h>
#endif
