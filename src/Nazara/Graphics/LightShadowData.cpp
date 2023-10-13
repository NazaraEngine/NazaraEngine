// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/LightShadowData.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	LightShadowData::~LightShadowData() = default;

	void LightShadowData::RegisterViewer(const AbstractViewer* /*viewer*/)
	{
	}

	void LightShadowData::UnregisterViewer(const AbstractViewer* /*viewer*/)
	{
	}
}
