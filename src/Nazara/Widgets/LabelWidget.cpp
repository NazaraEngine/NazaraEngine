// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	LabelWidget::LabelWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_style = GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void LabelWidget::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_style->UpdateText(drawer);

		Vector2f size(drawer.GetBounds().GetLengths());
		SetMinimumSize(size);
		SetPreferredSize(size + Vector2f(20.f, 10.f));

		Layout();
	}

	void LabelWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void LabelWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void LabelWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
