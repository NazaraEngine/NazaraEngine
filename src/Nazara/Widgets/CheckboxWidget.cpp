// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/CheckboxWidget.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/MaterialPass.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>

namespace Nz
{
	CheckboxWidget::CheckboxWidget(BaseWidget* parent, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_state(CheckboxState::Unchecked),
	m_isTristateEnabled(false)
	{
		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Layout();
	}

	void CheckboxWidget::SetState(CheckboxState checkboxState)
	{
		OnCheckboxStateUpdate(this, checkboxState);
		m_state = checkboxState;

		m_style->OnNewState(m_state);
	}

	void CheckboxWidget::Layout()
	{
		BaseWidget::Layout();
		m_style->Layout(GetSize());
	}

	bool CheckboxWidget::OnMouseButtonPress(int /*x*/, int /*y*/, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_style->OnPress();
			return true;
		}

		return false;
	}

	bool CheckboxWidget::OnMouseButtonRelease(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_style->OnRelease();

			// If user clicks inside button and holds it outside, a release mouse button event will be triggered outside of the widget
			// we don't want this to trigger the button, so double-check
			if (IsInside(float(x), float(y)))
				SwitchToNextState();

			return true;
		}

		return false;
	}

	void CheckboxWidget::OnMouseEnter()
	{
		m_style->OnHoverBegin();
	}

	void CheckboxWidget::OnMouseExit()
	{
		m_style->OnHoverEnd();
	}

	void CheckboxWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}
}
