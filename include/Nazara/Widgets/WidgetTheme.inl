// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <cassert>

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

	inline entt::entity BaseWidgetStyle::CreateGraphicsEntity(Node* parent)
	{
		return m_widgetOwner->CreateGraphicsEntity(parent);
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

