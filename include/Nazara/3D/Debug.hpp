// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - 3D Module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/3D/Config.hpp>
#if NAZARA_3D_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#include <Nazara/Core/Debug/MemoryLeakTracker.hpp>

	#define delete NzMemoryManager::NextFree(__FILE__, __LINE__), delete
	#define new new(__FILE__, __LINE__)
#endif
