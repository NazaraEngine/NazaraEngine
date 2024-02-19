// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/AbstractLabelWidget.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>

namespace Nz
{
	AbstractLabelWidget::AbstractLabelWidget(BaseWidget* parent) :
	BaseWidget(parent)
	{
		m_style = GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void AbstractLabelWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void AbstractLabelWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void AbstractLabelWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
