// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

namespace Ndk
{
	inline void ProgressBarWidget::EnableText(bool enable)
	{
		m_textEntity->Enable(enable);
		Layout();
	}

	inline void ProgressBarWidget::EnableBorder(bool enable)
	{
		m_borderEntity->Enable(enable);
	}

	inline bool ProgressBarWidget::IsTextEnabled() const
	{
		return m_textEntity->IsEnabled();
	}

	inline bool ProgressBarWidget::IsBorderEnabled() const
	{
		return m_borderEntity->IsEnabled();
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


	inline const Nz::Color& ProgressBarWidget::GetBarBackgroundColor() const
	{
		return m_barBackgroundSprite->GetColor();
	}

	inline const Nz::Color& ProgressBarWidget::GetBarBackgroundCornerColor() const
	{
		return m_barBackgroundSprite->GetCornerColor(Nz::RectCorner_LeftTop);
	}

	inline const Nz::Color& ProgressBarWidget::GetBarColor() const
	{
		return m_barSprite->GetColor();
	}

	inline const Nz::Color& ProgressBarWidget::GetBarCornerColor() const
	{
		return m_barSprite->GetCornerColor(Nz::RectCorner_LeftTop);
	}


	inline const Nz::TextureRef& ProgressBarWidget::GetBarBackgroundTexture() const
	{
		return m_barBackgroundSprite->GetMaterial()->GetDiffuseMap();
	}

	inline const Nz::TextureRef& ProgressBarWidget::GetBarTexture() const
	{
		return m_barSprite->GetMaterial()->GetDiffuseMap();
	}


	inline void ProgressBarWidget::SetBarBackgroundColor(const Nz::Color& globalColor, const Nz::Color& cornerColor)
	{
		m_barBackgroundSprite->SetColor(globalColor);
		m_barBackgroundSprite->SetCornerColor(Nz::RectCorner_LeftTop, cornerColor);
		m_barBackgroundSprite->SetCornerColor(Nz::RectCorner_RightTop, cornerColor);
		m_barBackgroundSprite->SetCornerColor(Nz::RectCorner_LeftBottom, globalColor);
		m_barBackgroundSprite->SetCornerColor(Nz::RectCorner_RightBottom, globalColor);
	}

	inline void ProgressBarWidget::SetBarBackgroundTexture(Nz::TextureRef texture, bool resetColors)
	{
		m_barBackgroundSprite->SetTexture(texture, false);

		if (resetColors)
			SetBarBackgroundColor(Nz::Color::White, Nz::Color::White);
	}

	inline void ProgressBarWidget::SetBarColor(const Nz::Color& globalColor, const Nz::Color& cornerColor)
	{
		m_barSprite->SetColor(globalColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_LeftTop, cornerColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_RightTop, cornerColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_LeftBottom, globalColor);
		m_barSprite->SetCornerColor(Nz::RectCorner_RightBottom, globalColor);
	}

	inline void ProgressBarWidget::SetBarTexture(Nz::TextureRef texture, bool resetColors)
	{
		m_barSprite->SetTexture(texture, false);

		if (resetColors)
			SetBarColor(Nz::Color::White, Nz::Color::White);
	}


	inline void ProgressBarWidget::SetPercentageValue(unsigned percentage)
	{
		m_value = percentage;
		OnValueChanged(this);
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
			m_textSprite->Update(Nz::SimpleTextDrawer::Draw(Nz::String::Number(m_value).Append('%'),
															static_cast<unsigned>(std::min(size.x, size.y) / 2.f), 0u, m_textColor));
		}
	}
}
