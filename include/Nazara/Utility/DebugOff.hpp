// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// no header guards

// On suppose que Debug.hpp a déjà été inclus, tout comme Config.hpp
#if NAZARA_UTILITY_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
