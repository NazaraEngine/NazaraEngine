// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline PluginInterface::PluginInterface() :
	m_isActive(false)
	{
	}

	inline bool PluginInterface::Activate()
	{
		if (m_isActive)
			return true;

		if (!ActivateImpl())
			return false;

		m_isActive = true;
		return true;
	}

	inline void PluginInterface::Deactivate()
	{
		if (!m_isActive)
			return;

		DeactivateImpl();
		m_isActive = false;
	}

	inline bool PluginInterface::IsActive() const
	{
		return m_isActive;
	}
}
