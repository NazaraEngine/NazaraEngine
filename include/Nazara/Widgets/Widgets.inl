// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<MaterialInstance>& Widgets::GetOpaqueMaterial() const
	{
		return m_opaqueMaterial;
	}

	inline const std::shared_ptr<MaterialInstance>& Widgets::GetTransparentMaterial() const
	{
		return m_transparentMaterial;
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
