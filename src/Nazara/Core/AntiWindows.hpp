// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

#ifndef _WINDOWS_
#error This header should only be included after including windows.h directly or indirectly in a .cpp
#endif

#undef CreateWindow
#undef MemoryBarrier
#undef RemoveDirectory
