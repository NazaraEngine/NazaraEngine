// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// On suppose que Debug.hpp a déjà été inclus, tout comme Config.hpp
#if NAZARA_PHYSICS3D_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
