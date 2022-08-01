// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/ImageButtonWidget.hpp>
#include <Nazara/Graphics/BasicMaterial.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	ImageButtonWidget::ImageButtonWidget(BaseWidget* parent, std::shared_ptr<Material> material, std::shared_ptr<Material> hoveredMaterial, std::shared_ptr<Material> pressedMaterial, float cornerSize, float cornerTexCoords) :
	BaseWidget(parent),
	m_hoveredMaterial(std::move(hoveredMaterial)),
	m_material(std::move(material)),
	m_pressedMaterial(std::move(pressedMaterial)),
	m_color(Color::White),
	m_textureCoords(0.f, 0.f, 1.f, 1.f),
	m_cornerSize(cornerSize),
	m_cornerTexCoords(cornerTexCoords)
	{
		m_style = GetTheme()->CreateStyle(this);
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
		if (const auto& material = m_material->FindPass("ForwardPass"))
		{
			BasicMaterial mat(*material);
			if (mat.HasBaseColorMap())
			{
				// Material should always have textures but we're better safe than sorry
				if (const auto& texture = mat.GetBaseColorMap())
				{
					Vector2f textureSize = Vector2f(Vector2ui(texture->GetSize()));
					textureSize.x *= textureCoords.width;
					textureSize.y *= textureCoords.height;

					SetPreferredSize(textureSize);
				}
			}
		}
	}
}
