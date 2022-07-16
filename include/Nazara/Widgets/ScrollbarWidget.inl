// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ScrollbarWidget.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <Nazara/Widgets/Debug.hpp>

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

		Layout();
	}
}

#include <Nazara/Widgets/DebugOff.hpp>
