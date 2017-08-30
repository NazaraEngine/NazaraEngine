// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// We suppose that Debug.hpp is already included, same goes for Config.hpp
#if NAZARA_PLATFORM_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
