// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderImage.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	RenderImage::~RenderImage()
	{
		FlushReleaseQueue();
	}

	RenderImage::Releasable::~Releasable() = default;
}
