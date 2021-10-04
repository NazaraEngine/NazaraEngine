// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Widgets/LabelWidget.hpp>

namespace Nz
{
	inline void LabelWidget::UpdateText(const Nz::AbstractTextDrawer& drawer, float scale)
	{
		m_textSprite->Update(drawer, scale);

		Nz::Vector2f size = Nz::Vector2f(m_textSprite->GetAABB().GetLengths());
		SetMinimumSize(size);
		SetPreferredSize(size);
	}
}
