// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline const std::shared_ptr<Material>& Widgets::GetOpaqueMaterial() const
	{
		return m_opaqueMaterial;
	}

	inline const std::shared_ptr<MaterialPass>& Widgets::GetOpaqueMaterialPass() const
	{
		return m_opaqueMaterialPass;
	}

	inline const std::shared_ptr<Material>& Widgets::GetTransparentMaterial() const
	{
		return m_transparentMaterial;
	}

	inline const std::shared_ptr<MaterialPass>& Widgets::GetTransparentMaterialPass() const
	{
		return m_transparentMaterialPass;
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
