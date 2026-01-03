// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERIMAGE_HPP
#define NAZARA_RENDERER_RENDERIMAGE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/RenderResources.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API RenderImage : public RenderResources
	{
		public:
			using RenderResources::RenderResources;

			virtual void Present() = 0;
	};
}

#include <Nazara/Renderer/RenderImage.inl>

#endif // NAZARA_RENDERER_RENDERIMAGE_HPP
