// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_PLUGINS_IMGUIFUNCTIONS_HPP
#define NAZARA_RENDERER_PLUGINS_IMGUIFUNCTIONS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/FunctionTraits.hpp>
#include <imgui.h>

namespace Nz
{
	struct ImGuiFunctions
	{
#define NAZARA_RENDERER_PLUGINS_IMGUI_FUNC(func) \
		decltype(&ImGui::func) func;
#include <Nazara/Renderer/Plugins/ImGuiFunctionList.hpp>
	};
}

#endif // NAZARA_RENDERER_PLUGINS_IMGUIFUNCTIONS_HPP
