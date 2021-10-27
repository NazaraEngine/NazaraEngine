// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/LabelWidget.hpp>
#include <Nazara/Widgets/Debug.hpp>

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

#include <Nazara/Widgets/DebugOff.hpp>
