// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Widgets/LabelWidget.hpp>

namespace Ndk
{
	inline void LabelWidget::UpdateText(const Nz::AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);

		Nz::Vector2f size = Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths());
		SetMinimumSize(size);
		SetPreferredSize(size);
	}
}
