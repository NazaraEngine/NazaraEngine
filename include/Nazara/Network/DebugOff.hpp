// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// We suppose that Debug.hpp is already included, same goes for Config.hpp
#if NAZARA_NETWORK_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
