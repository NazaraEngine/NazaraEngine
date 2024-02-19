// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>

namespace Nz
{
	void LabelWidget::UpdateText(const AbstractTextDrawer& drawer)
	{
		m_style->UpdateText(drawer);

		Vector2f size(drawer.GetBounds().GetLengths());
		SetMinimumSize(size);
		SetPreferredSize(size);

		Layout();
	}
}
