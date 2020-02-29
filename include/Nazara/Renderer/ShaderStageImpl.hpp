// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_SHADERSTAGEIMPL_HPP
#define NAZARA_RENDERER_SHADERSTAGEIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API ShaderStageImpl
	{
		public:
			ShaderStageImpl() = default;
			virtual ~ShaderStageImpl();
	};
}

#endif // NAZARA_RENDERER_SHADERSTAGEIMPL_HPP
