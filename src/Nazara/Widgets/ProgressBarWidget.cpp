// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ProgressBarWidget.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	ProgressBarWidget::ProgressBarWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_fraction(0.f)
	{
		m_style = GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void ProgressBarWidget::Layout()
	{
		BaseWidget::Layout();
		m_style->Layout(GetSize());
	}

	void ProgressBarWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
