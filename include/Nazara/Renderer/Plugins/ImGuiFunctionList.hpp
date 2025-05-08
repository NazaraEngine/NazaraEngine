// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// no header guards

#ifndef NAZARA_DOCGEN

#ifndef NAZARA_RENDERER_PLUGINS_IMGUI_FUNC
#error You must define NAZARA_RENDERER_PLUGINS_IMGUI_FUNC before including this file
#endif

#ifndef NAZARA_RENDERER_PLUGINS_IMGUI_FUNC_LAST
#define NAZARA_RENDERER_PLUGINS_IMGUI_FUNC_LAST NAZARA_RENDERER_PLUGINS_IMGUI_FUNC
#endif

NAZARA_RENDERER_PLUGINS_IMGUI_FUNC(Begin)
NAZARA_RENDERER_PLUGINS_IMGUI_FUNC(Button)
NAZARA_RENDERER_PLUGINS_IMGUI_FUNC(End)
NAZARA_RENDERER_PLUGINS_IMGUI_FUNC(Separator)
NAZARA_RENDERER_PLUGINS_IMGUI_FUNC_LAST(Text)

#undef NAZARA_RENDERER_PLUGINS_IMGUI_FUNC
#undef NAZARA_RENDERER_PLUGINS_IMGUI_FUNC_LAST

#endif // NAZARA_DOCGEN
