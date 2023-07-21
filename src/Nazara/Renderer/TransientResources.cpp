// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/TransientResources.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	TransientResources::~TransientResources()
	{
		FlushReleaseQueue();
	}

	TransientResources::Releasable::~Releasable() = default;
}
