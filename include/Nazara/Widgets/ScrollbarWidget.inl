// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Algorithm.hpp>

namespace Nz
{
	inline ScrollbarOrientation ScrollbarWidget::GetOrientation() const
	{
		return m_orientation;
	}

	inline float ScrollbarWidget::GetMaximumValue() const
	{
		return m_maximumValue;
	}

	inline float ScrollbarWidget::GetMinimumValue() const
	{
		return m_minimumValue;
	}

	inline float ScrollbarWidget::GetStep() const
	{
		return m_step;
	}

	inline float ScrollbarWidget::GetValue() const
	{
		return m_value;
	}

	inline void ScrollbarWidget::SetValue(float newValue)
	{
		m_value = Clamp(newValue, m_minimumValue, m_maximumValue);
		OnScrollbarValueUpdate(this, m_value);

		Layout();
	}
}

