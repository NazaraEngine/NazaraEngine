// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{

	inline void CheckboxWidget::UpdateText(const Nz::AbstractTextDrawer& drawer)
	{
		m_textSprite->Update(drawer);
		Layout();
	}

	inline void CheckboxWidget::SetCheckboxSize(const Nz::Vector2ui& size)
	{
		m_size = size;
	}

	inline void CheckboxWidget::SetCheckboxBorderSize(const Nz::Vector2ui& size)
	{
		m_borderSize = size;
	}


	inline void CheckboxWidget::EnableTristate(bool enable)
	{
		m_tristate = enable;
	}

	inline bool CheckboxWidget::IsTristateEnabled() const
	{
		return m_tristate;
	}


	inline CheckboxState CheckboxWidget::GetState() const
	{
		return m_state;
	}

	inline bool CheckboxWidget::ContainsCheckbox(int x, int y) const
	{
		Nz::Vector3f pos = m_checkboxEntity->GetComponent<Ndk::NodeComponent>().GetPosition();
		return x > pos.x && x < pos.x + m_size.x && y > pos.y && y < pos.y + m_size.y;
	}
}
