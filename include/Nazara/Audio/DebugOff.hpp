// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// We assume that Debug.hpp has already been included, same thing for Config.hpp
#if NAZARA_AUDIO_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
