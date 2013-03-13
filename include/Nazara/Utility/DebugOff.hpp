// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_UTILITY_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
