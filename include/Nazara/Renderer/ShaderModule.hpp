// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_SHADERMODULE_HPP
#define NAZARA_RENDERER_SHADERMODULE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <string_view>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderModule
	{
		public:
			ShaderModule() = default;
			virtual ~ShaderModule();

			virtual void UpdateDebugName(std::string_view name) = 0;
	};
}

#endif // NAZARA_RENDERER_SHADERMODULE_HPP
