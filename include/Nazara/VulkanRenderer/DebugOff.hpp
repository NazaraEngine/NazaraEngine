// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

// On suppose que Debug.hpp a déjà été inclus, tout comme Config.hpp
#if NAZARA_VULKANRENDERER_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
