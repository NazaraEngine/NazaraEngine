// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline ApplicationComponent::ApplicationComponent(ApplicationBase& app) :
	m_app(app)
	{
	}

	inline ApplicationBase& ApplicationComponent::GetApp()
	{
		return m_app;
	}

	inline const ApplicationBase& ApplicationComponent::GetApp() const
	{
		return m_app;
	}
}

#include <Nazara/Core/DebugOff.hpp>
