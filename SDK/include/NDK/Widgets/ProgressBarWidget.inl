// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline void ProgressBarWidget::EnableText(bool enable)
	{
		m_textEntity->Enable(enable);

		UpdateText();
		Layout();
	}

	inline bool ProgressBarWidget::IsTextEnabled() const
	{
		return m_textEntity->IsEnabled();
	}


	inline unsigned ProgressBarWidget::GetPercentageValue() const
	{
		return m_value;
	}

	inline Nz::Vector2f ProgressBarWidget::GetProgressBarSize() const
	{
		Nz::Vector3f progressBarSize = m_borderSprite->GetBoundingVolume().obb.localBox.GetLengths();

		if (IsTextEnabled())
		{
			Nz::Vector3f textSize = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
			progressBarSize -= { textSize.x + m_textMargin, 0.f, 0.f };
		}

		return { progressBarSize.x, progressBarSize.y };
	}

	inline Nz::Vector2f ProgressBarWidget::GetProgressBarBorderSize() const
	{
		Nz::Vector2f barSize = GetProgressBarSize();
		return { barSize.y / s_borderScale, barSize.y / s_borderScale };
	}

	inline float ProgressBarWidget::GetTextMargin() const
	{
		return m_textMargin;
	}


	inline void ProgressBarWidget::SetPercentageValue(unsigned percentage)
	{
		m_value = percentage;
		OnValueChanged(this);

		UpdateText();
		Layout();
	}

	inline void ProgressBarWidget::SetTextMargin(float margin)
	{
		m_textMargin = margin;
		
		if (IsTextEnabled())
			Layout();
	}

	inline void ProgressBarWidget::SetTextColor(const Nz::Color& color)
	{
		m_textColor = color;
		UpdateText();
	}

	inline void ProgressBarWidget::UpdateText()
	{
		if (IsTextEnabled())
		{
			Nz::Vector2f size = GetContentSize();
			m_textSprite->Update(Nz::SimpleTextDrawer::Draw(Nz::String::Number(m_value).Append('%'), static_cast<unsigned>(size.y / 2.f), 0u, m_textColor));
		}
	}
}
