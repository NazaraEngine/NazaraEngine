// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <algorithm>

namespace Nz
{
	inline float ProgressBarWidget::GetFraction() const
	{
		return m_fraction;
	}

	inline void ProgressBarWidget::SetFraction(float fraction)
	{
		m_fraction = std::clamp(fraction, 0.f, 1.f);
		Layout();
	}
}
