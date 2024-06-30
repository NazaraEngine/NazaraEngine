// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline ImageButtonWidget::ImageButtonWidget(BaseWidget* parent, std::shared_ptr<MaterialInstance> material, const StyleFactory& styleFactory) :
	ImageButtonWidget(parent, std::move(material), {}, {}, 0.f, 0.f, styleFactory)
	{
	}

	inline ImageButtonWidget::ImageButtonWidget(BaseWidget* parent, std::shared_ptr<MaterialInstance> material, float cornerSize, float cornerTexCoords, const StyleFactory& styleFactory) :
	ImageButtonWidget(parent, std::move(material), {}, {}, cornerSize, cornerTexCoords, styleFactory)
	{
	}

	inline const Color& ImageButtonWidget::GetColor() const
	{
		return m_color;
	}

	inline float ImageButtonWidget::GetCornerSize() const
	{
		return m_cornerSize;
	}

	inline float ImageButtonWidget::GetCornerTexCoords() const
	{
		return m_cornerTexCoords;
	}

	inline const std::shared_ptr<MaterialInstance>& ImageButtonWidget::GetHoveredMaterial() const
	{
		return m_hoveredMaterial;
	}

	inline const std::shared_ptr<MaterialInstance>& ImageButtonWidget::GetMaterial() const
	{
		return m_material;
	}

	inline const std::shared_ptr<MaterialInstance>& ImageButtonWidget::GetPressedMaterial() const
	{
		return m_pressedMaterial;
	}

	inline const Rectf& ImageButtonWidget::GetTextureCoords() const
	{
		return m_textureCoords;
	}

	inline void ImageButtonWidget::SetColor(const Color& color)
	{
		m_color = color;

		m_style->OnUpdate();
	}

	inline void ImageButtonWidget::SetCorner(float size, float texcoords)
	{
		m_cornerSize = size;
		m_cornerTexCoords = texcoords;

		m_style->OnUpdate();
	}

	inline void ImageButtonWidget::SetHoveredMaterial(std::shared_ptr<MaterialInstance> material)
	{
		m_hoveredMaterial = std::move(material);

		m_style->OnUpdate();
	}

	inline void ImageButtonWidget::SetMaterial(std::shared_ptr<MaterialInstance> material)
	{
		m_material = std::move(material);
		UpdatePreferredSize();

		m_style->OnUpdate();
	}

	inline void ImageButtonWidget::SetPressedMaterial(std::shared_ptr<MaterialInstance> material)
	{
		m_pressedMaterial = std::move(material);

		m_style->OnUpdate();
	}

	inline void ImageButtonWidget::SetTextureCoords(const Rectf& coords)
	{
		m_textureCoords = coords;
		UpdatePreferredSize();

		m_style->OnUpdate();
	}
}

