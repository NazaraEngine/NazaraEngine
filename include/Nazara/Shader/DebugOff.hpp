// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

// We suppose that Debug.hpp is already included, same goes for Config.hpp
#if NAZARA_SHADER_MANAGE_MEMORY
	#undef delete
	#undef new
#endif
