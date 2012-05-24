// Copyright (C) 2012 Jérôme "Lynix" Leclercq (Lynix680@gmail.com)
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_NOISE_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
