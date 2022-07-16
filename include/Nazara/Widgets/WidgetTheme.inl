// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/WidgetTheme.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <cassert>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	inline auto WidgetTheme::GetConfig() const -> const Config&
	{
		return m_config;
	}


	inline BaseWidgetStyle::BaseWidgetStyle(BaseWidget* widget, int renderLayerCount) :
	m_widgetOwner(widget),
	m_renderLayerCount(renderLayerCount)
	{
		assert(m_renderLayerCount >= 0);
	}

	inline entt::entity BaseWidgetStyle::CreateEntity()
	{
		return m_widgetOwner->CreateEntity();
	}

	inline void BaseWidgetStyle::DestroyEntity(entt::entity entity)
	{
		m_widgetOwner->DestroyEntity(entity);
	}

	template<typename T>
	T* BaseWidgetStyle::GetOwnerWidget() const
	{
		return SafeCast<T*>(m_widgetOwner);
	}

	inline entt::registry& BaseWidgetStyle::GetRegistry()
	{
		return m_widgetOwner->GetRegistry();
	}

	inline const entt::registry& BaseWidgetStyle::GetRegistry() const
	{
		return m_widgetOwner->GetRegistry();
	}

	inline int BaseWidgetStyle::GetRenderLayerCount() const
	{
		return m_renderLayerCount;
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
