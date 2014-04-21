// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_RENDERER_MEMORYMANAGER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
