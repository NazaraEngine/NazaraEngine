// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/CheckboxWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/World.hpp>
#include <algorithm>

namespace Ndk
{
	CheckboxWidget::CheckboxWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_tristate { false },
	m_checkboxEnabled { true },
	m_adaptativeMargin { true },
	m_textMargin { 16.f },
	m_size { 32, 32 },
	m_borderSize { 3, 3 }
	{
		m_checkboxSprite = Nz::Sprite::New();
		m_checkboxTextSprite = Nz::TextSprite::New();
		m_textSprite = Nz::TextSprite::New();

		m_checkboxEntity = CreateEntity();
		m_checkboxEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxSprite);

		m_checkboxTextEntity = CreateEntity();
		m_checkboxTextEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxTextEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxTextSprite, 1);

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);

		UpdateCheckboxSprite();
		Layout();
	}

	void CheckboxWidget::ResizeToContent()
	{
		Nz::Vector2f textSize = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		Nz::Vector2f size { std::max(textSize.x, static_cast<float>(m_size.x)), std::max(textSize.y, static_cast<float>(m_size.y)) };

		SetContentSize(size);
	}


	void CheckboxWidget::SetState(CheckboxState state)
	{
		if (!m_checkboxEnabled)
			return;

		if (state == CheckboxState_Tristate)
			m_tristate = true;

		m_state = state;

		UpdateCheckboxSprite();
	}

	CheckboxState CheckboxWidget::SetNextState()
	{
		if (!m_checkboxEnabled)
			return m_state;

		switch (m_state)
		{
			case CheckboxState_Unchecked:
				SetState(CheckboxState_Checked);
				break;

			case CheckboxState_Checked:
				SetState(m_tristate ? CheckboxState_Tristate : CheckboxState_Unchecked);
				break;

			case CheckboxState_Tristate:
				SetState(CheckboxState_Unchecked);
				break;
		}

		return m_state;
	}


	void CheckboxWidget::Layout()
	{
		BaseWidget::Layout();

		Nz::Vector2f origin = GetContentOrigin();
		m_checkboxEntity->GetComponent<NodeComponent>().SetPosition(origin);

		Nz::Vector2f checkboxTextBox = m_checkboxTextSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_checkboxTextEntity->GetComponent<NodeComponent>().SetPosition(origin.x + m_borderSize.x, origin.y + m_borderSize.y);

		Nz::Vector2f textBox = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_textEntity->GetComponent<NodeComponent>().SetPosition(origin.x + static_cast<float>(m_size.x) + static_cast<float>(m_adaptativeMargin ? m_size.x / 2 : m_textMargin),
																origin.y + m_size.y / 2 - textBox.y / 2);
	}

	void CheckboxWidget::OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left && ContainsCheckbox(x, y))
		{
			SetNextState();
			OnStateChanged(this);
		}
	}

	void CheckboxWidget::UpdateCheckboxSprite()
	{
		Nz::Image checkbox;
		checkbox.Create(Nz::ImageType_2D, Nz::PixelFormatType_L8, m_size.x, m_size.y);

		if (m_checkboxEnabled)
		{
			checkbox.Fill(Nz::Color::Black, Nz::Rectui { m_size.x, m_size.y });
			checkbox.Fill(Nz::Color::White, Nz::Rectui { m_borderSize.x, m_borderSize.y, m_size.x - (m_borderSize.x * 2), m_size.y - (m_borderSize.y * 2) });
		}
		else
		{
			checkbox.Fill(Nz::Color { 62, 62, 62 }, Nz::Rectui { m_size.x, m_size.y });
			checkbox.Fill(Nz::Color { 201, 201, 201 }, Nz::Rectui { m_borderSize.x, m_borderSize.y, m_size.x - (m_borderSize.x * 2), m_size.y - (m_borderSize.y * 2) });
		}

		m_checkboxSprite->SetTexture(Nz::Texture::New(checkbox).Get());
		m_checkboxSprite->SetSize(static_cast<float>(m_size.x), static_cast<float>(m_size.y));

		Nz::String checkboxString;

		if (m_state == CheckboxState_Checked)
			checkboxString.Set(u8"v"); // ✓

		if (m_state == CheckboxState_Tristate)
			checkboxString.Set(u8"o"); // ■

		m_checkboxTextSprite->Update(Nz::SimpleTextDrawer::Draw(checkboxString, (m_size.x + m_size.y) / 2u - (m_borderSize.x + m_borderSize.y) / 2u - 2u,
																0u, Nz::Color::Black));
	}
}
