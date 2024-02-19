// Copyright (C) 2024 Samy Bensaid
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline const Color& ImageWidget::GetColor() const
	{
		return m_sprite->GetColor();
	}

	inline const std::shared_ptr<MaterialInstance>& ImageWidget::GetMaterial() const
	{
		return m_sprite->GetMaterial();
	}

	inline const Rectf& ImageWidget::GetTextureCoords() const
	{
		return m_sprite->GetTextureCoords();
	}

	inline void ImageWidget::SetColor(const Color& color)
	{
		m_sprite->SetColor(color);
	}

	inline void ImageWidget::SetMaterial(const std::shared_ptr<MaterialInstance>& texture)
	{
		m_sprite->SetMaterial(texture);
		UpdatePreferredSize();
	}

	inline void ImageWidget::SetTextureCoords(const Rectf& coords)
	{
		m_sprite->SetTextureCoords(coords);
		UpdatePreferredSize();
	}

	inline void ImageWidget::SetTextureRect(const Rectf& rect)
	{
		m_sprite->SetTextureRect(rect);
		UpdatePreferredSize();
	}

	inline void ImageWidget::UpdatePreferredSize()
	{
		const Rectf& textureCoords = GetTextureCoords();

		Vector2f textureSize = Vector2f(Vector2ui(m_sprite->GetTextureSize()));
		textureSize.x *= textureCoords.width;
		textureSize.y *= textureCoords.height;

		SetPreferredSize(textureSize);
	}
}

