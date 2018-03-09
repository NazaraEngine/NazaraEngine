// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERSURFACE_HPP
#define NAZARA_RENDERSURFACE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	///TODO: Rename
	class NAZARA_RENDERER_API RenderSurface
	{
		public:
			RenderSurface() = default;
			virtual ~RenderSurface();

			virtual bool Create(WindowHandle handle) = 0;
			virtual void Destroy() = 0;
	};
}

#include <Nazara/Renderer/RenderSurface.inl>

#endif // NAZARA_RENDERSURFACE_HPP
