// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERIMAGE_HPP
#define NAZARA_RENDERER_RENDERIMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/TransientResources.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API RenderImage : public TransientResources
	{
		public:
			virtual void Present() = 0;
	};
}

#include <Nazara/Renderer/RenderImage.inl>

#endif // NAZARA_RENDERER_RENDERIMAGE_HPP
