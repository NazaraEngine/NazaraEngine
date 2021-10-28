// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthMaterial.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<MaterialSettings>& DepthMaterial::GetSettings()
	{
		return s_materialSettings;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
