// Copyright (C) 2012 Rémi "Overdrivr" Bèges (remi.beges@laposte.net)
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#if NAZARA_NOISE_MEMORYLEAKTRACKER || defined(NAZARA_DEBUG)
	#undef delete
	#undef new
#endif
