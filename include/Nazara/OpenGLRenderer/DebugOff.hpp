// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

// On suppose que Debug.hpp a déjà été inclus, tout comme Config.hpp
#if NAZARA_OPENGLRENDERER_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
