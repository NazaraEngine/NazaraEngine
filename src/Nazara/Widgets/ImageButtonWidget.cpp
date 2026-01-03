// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/ImageButtonWidget.hpp>
#include <Nazara/Graphics/MaterialInstance.hpp>
#include <Nazara/Graphics/TextureAsset.hpp>

namespace Nz
{
	ImageButtonWidget::ImageButtonWidget(BaseWidget* parent, std::shared_ptr<MaterialInstance> material, std::shared_ptr<MaterialInstance> hoveredMaterial, std::shared_ptr<MaterialInstance> pressedMaterial, float cornerSize, float cornerTexCoords, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_hoveredMaterial(std::move(hoveredMaterial)),
	m_material(std::move(material)),
	m_pressedMaterial(std::move(pressedMaterial)),
	m_color(Color::White()),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_cornerSize(cornerSize),
	m_cornerTexCoords(cornerTexCoords)
	{
		m_hoveredTextureCoords = m_textureCoords;
		m_pressedTextureCoords = m_textureCoords;

		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		UpdatePreferredSize();
		Layout();
	}

	ImageButtonWidget::ImageButtonWidget(BaseWidget* parent, std::shared_ptr<MaterialInstance> material, const Rectf& coords, const Rectf& hoveredCoords, const Rectf& pressedCoords, float cornerSize, float cornerTexCoords, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_hoveredMaterial(material),
	m_material(material),
	m_pressedMaterial(std::move(material)),
	m_color(Color::White()),
	m_hoveredTextureCoords(hoveredCoords),
	m_pressedTextureCoords(pressedCoords),
	m_textureCoords(coords),
	m_cornerSize(cornerSize),
	m_cornerTexCoords(cornerTexCoords)
	{
		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		UpdatePreferredSize();
		Layout();
	}

	void ImageButtonWidget::Layout()
	{
		BaseWidget::Layout();
		m_style->Layout(GetSize());
	}

	bool ImageButtonWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_style->OnPress();
			return true;
		}

		return false;
	}

	bool ImageButtonWidget::OnMouseButtonRelease(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_style->OnRelease();

			// If user clicks inside button and holds it outside, a release mouse button event will be triggered outside of the widget
			// we don't want this to trigger the button, so double-check
			if (IsInside(float(x), float(y)))
				OnButtonTrigger(this);

			return true;
		}

		return false;
	}

	void ImageButtonWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void ImageButtonWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void ImageButtonWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}

	void ImageButtonWidget::UpdatePreferredSize()
	{
		const Rectf& textureCoords = GetTextureCoords();

		// TODO: Move this in a separate function
		if (const std::shared_ptr<TextureAsset>* textureOpt = m_material->GetTextureProperty("BaseColorMap"))
		{
			// Material should always have textures but we're better safe than sorry
			if (const std::shared_ptr<TextureAsset>& texture = *textureOpt)
			{
				Vector2f textureSize = Vector2f(Vector2ui(texture->GetSize()));
				textureSize.x *= textureCoords.width;
				textureSize.y *= textureCoords.height;

				SetPreferredSize(textureSize);
			}
		}
	}
}
