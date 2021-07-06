// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTVIEWER_HPP
#define NAZARA_ABSTRACTVIEWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class RenderTarget;
	class ViewerInstance;

	class NAZARA_GRAPHICS_API AbstractViewer
	{
		public:
			AbstractViewer() = default;
			~AbstractViewer() = default;

			virtual const RenderTarget& GetRenderTarget() = 0;
			virtual ViewerInstance& GetViewerInstance() = 0;
			virtual const ViewerInstance& GetViewerInstance() const = 0;
	};
}

#include <Nazara/Graphics/AbstractViewer.inl>

#endif
