// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <functional>

namespace Nz
{
	inline TextureSamplerCache::TextureSamplerCache(std::shared_ptr<RenderDevice> device) :
	m_device(std::move(device))
	{
	}
}

