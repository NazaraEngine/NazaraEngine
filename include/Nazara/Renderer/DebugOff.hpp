// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_RENDERER_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
