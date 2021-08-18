// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_SHADERMODULE_HPP
#define NAZARA_RENDERER_SHADERMODULE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderModule
	{
		public:
			ShaderModule() = default;
			virtual ~ShaderModule();
	};
}

#endif // NAZARA_RENDERER_SHADERSTAGE_HPP
