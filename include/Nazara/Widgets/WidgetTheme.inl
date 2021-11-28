// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/WidgetTheme.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline BaseWidgetStyle::BaseWidgetStyle(BaseWidget* widget) :
	m_widgetOwner(widget)
	{
	}

	inline entt::entity BaseWidgetStyle::CreateEntity()
	{
		return m_widgetOwner->CreateEntity();
	}

	inline void BaseWidgetStyle::DestroyEntity(entt::entity entity)
	{
		m_widgetOwner->DestroyEntity(entity);
	}

	inline entt::registry& BaseWidgetStyle::GetRegistry()
	{
		return m_widgetOwner->GetRegistry();
	}

	inline const entt::registry& BaseWidgetStyle::GetRegistry() const
	{
		return m_widgetOwner->GetRegistry();
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
