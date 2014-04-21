// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_PHYSICS_MEMORYMANAGER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
