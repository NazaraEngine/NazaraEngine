// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline void CheckboxWidget::EnableTristate(bool enabled)
	{
		m_isTristateEnabled = enabled;
	}

	inline CheckboxState CheckboxWidget::GetState() const
	{
		return m_state;
	}

	inline bool CheckboxWidget::IsTristateEnabled() const
	{
		return m_isTristateEnabled;
	}

	void CheckboxWidget::SetState(bool checkboxState)
	{
		return SetState((checkboxState) ? CheckboxState::Checked : CheckboxState::Unchecked);
	}

	inline void CheckboxWidget::SwitchToNextState()
	{
		switch (m_state)
		{
			case CheckboxState::Checked:
				SetState(CheckboxState::Unchecked);
				break;

			case CheckboxState::Unchecked:
			{
				SetState((m_isTristateEnabled) ? CheckboxState::Tristate : CheckboxState::Checked);
				break;
			}

			case CheckboxState::Tristate:
				SetState(CheckboxState::Checked);
				break;
		}
	}
}
