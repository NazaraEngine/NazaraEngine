// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_ABSTRACTVIEWER_HPP
#define NAZARA_GRAPHICS_ABSTRACTVIEWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Math/Rect.hpp>

namespace Nz
{
	class RenderTarget;
	class ViewerInstance;

	class NAZARA_GRAPHICS_API AbstractViewer
	{
		public:
			AbstractViewer() = default;
			~AbstractViewer() = default;

			virtual const Color& GetClearColor() const = 0;
			virtual UInt32 GetRenderMask() const = 0;
			virtual const RenderTarget& GetRenderTarget() const = 0;
			virtual ViewerInstance& GetViewerInstance() = 0;
			virtual const ViewerInstance& GetViewerInstance() const = 0;
			virtual const Recti& GetViewport() const = 0;
	};
}

#include <Nazara/Graphics/AbstractViewer.inl>

#endif // NAZARA_GRAPHICS_ABSTRACTVIEWER_HPP
