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


	inline void CheckboxWidget::SetTextMargin(float margin)
	{
		m_textMargin = margin;
		Layout();
	}

	inline float CheckboxWidget::GetTextMargin() const
	{
		return m_textMargin;
	}

	inline void CheckboxWidget::EnableAdaptativeMargin(bool enable)
	{
		m_adaptativeMargin = enable;
		Layout();
	}

	inline bool CheckboxWidget::IsMarginAdaptative() const
	{
		return m_adaptativeMargin;
	}


	inline void CheckboxWidget::SetCheckboxSize(const Nz::Vector2ui& size)
	{
		m_size = size;

		UpdateCheckboxSprite();
		Layout();
	}

	inline void CheckboxWidget::SetCheckboxBorderSize(const Nz::Vector2ui& size)
	{
		m_borderSize = size;

		UpdateCheckboxSprite();
		Layout();
	}

	inline const Nz::Vector2ui& CheckboxWidget::GetCheckboxSize() const
	{
		return m_size;
	}

	inline const Nz::Vector2ui& CheckboxWidget::GetCheckboxBorderSize() const
	{
		return m_borderSize;
	}


	inline void CheckboxWidget::EnableCheckbox(bool enable)
	{
		m_checkboxEnabled = enable;
		UpdateCheckboxSprite();
	}

	inline bool CheckboxWidget::IsCheckboxEnabled() const
	{
		return m_checkboxEnabled;
	}


	inline void CheckboxWidget::EnableTristate(bool enable)
	{
		m_tristateEnabled = enable;
	}

	inline bool CheckboxWidget::IsTristateEnabled() const
	{
		return m_tristateEnabled;
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
