// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/SimpleLabelWidget.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>

namespace Nz
{
	void SimpleLabelWidget::UpdateText()
	{
		m_style->UpdateText(m_drawer);
	}

	void SimpleLabelWidget::UpdateTextAndSize()
	{
		m_style->UpdateText(m_drawer);

		Vector2f size(m_drawer.GetBounds().GetLengths());
		SetMinimumSize(size);
		SetPreferredSize(size);

		Layout();
	}
}
