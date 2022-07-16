// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/WidgetTheme.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	WidgetTheme::~WidgetTheme() = default;

	BaseWidgetStyle::~BaseWidgetStyle() = default;

	entt::entity BaseWidgetStyle::CreateGraphicsEntity()
	{
		auto& registry = GetRegistry();

		entt::entity entity = CreateEntity();
		registry.emplace<GraphicsComponent>(entity, m_widgetOwner->IsVisible());
		registry.emplace<NodeComponent>(entity).SetParent(m_widgetOwner);

		return entity;
	}

	UInt32 BaseWidgetStyle::GetRenderMask() const
	{
		return m_widgetOwner->GetCanvas()->GetRenderMask();
	}


	void ButtonWidgetStyle::OnHoverBegin()
	{
	}

	void ButtonWidgetStyle::OnHoverEnd()
	{
	}

	void ButtonWidgetStyle::OnPress()
	{
	}

	void ButtonWidgetStyle::OnRelease()
	{
	}


	void CheckboxWidgetStyle::OnHoverBegin()
	{
	}

	void CheckboxWidgetStyle::OnHoverEnd()
	{
	}

	void CheckboxWidgetStyle::OnNewState(CheckboxState /*newState*/)
	{
	}

	void CheckboxWidgetStyle::OnPress()
	{
	}

	void CheckboxWidgetStyle::OnRelease()
	{
	}


	void ImageButtonWidgetStyle::OnHoverBegin()
	{
	}

	void ImageButtonWidgetStyle::OnHoverEnd()
	{
	}

	void ImageButtonWidgetStyle::OnPress()
	{
	}

	void ImageButtonWidgetStyle::OnRelease()
	{
	}


	void LabelWidgetStyle::OnHoverBegin()
	{
	}

	void LabelWidgetStyle::OnHoverEnd()
	{
	}


	void ScrollbarWidgetStyle::OnButtonGrab()
	{
	}

	void ScrollbarWidgetStyle::OnButtonRelease()
	{
	}

	void ScrollbarWidgetStyle::OnHoverBegin()
	{
	}

	void ScrollbarWidgetStyle::OnHoverEnd()
	{
	}


	void ScrollbarButtonWidgetStyle::OnHoverBegin()
	{
	}

	void ScrollbarButtonWidgetStyle::OnHoverEnd()
	{
	}

	void ScrollbarButtonWidgetStyle::OnGrab()
	{
	}

	void ScrollbarButtonWidgetStyle::OnRelease()
	{
	}
}
