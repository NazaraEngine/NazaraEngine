// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/CheckboxWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/World.hpp>

namespace Ndk
{
	CheckboxWidget::CheckboxWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_tristate { false },
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
		if (state == CheckboxState_Tristate)
			m_tristate = true;

		m_state = state;

		UpdateCheckboxSprite();
	}

	void CheckboxWidget::SetNextState()
	{
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
	}


	void CheckboxWidget::Layout()
	{
		BaseWidget::Layout();

		Nz::Vector2f origin = GetContentOrigin();
		m_checkboxEntity->GetComponent<NodeComponent>().SetPosition(origin);

		Nz::Boxf checkboxTextBox = m_checkboxTextEntity->GetComponent<GraphicsComponent>().GetBoundingVolume().aabb;
		m_checkboxTextEntity->GetComponent<NodeComponent>().SetPosition(origin.x + m_size.x / 2 - checkboxTextBox.width, origin.y + m_size.y / 2 - checkboxTextBox.height);

		Nz::Boxf textBox = m_textEntity->GetComponent<GraphicsComponent>().GetBoundingVolume().aabb;
		m_textEntity->GetComponent<NodeComponent>().SetPosition(origin.x + m_size.x + m_size.x / 2u, origin.y + m_size.y / 2 - textBox.height);

		NazaraDebug(Nz::String::Number(m_size.y).Append(" + ").Append(Nz::String::Number(textBox.height)));
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

		checkbox.Fill(Nz::Color::Black, Nz::Rectui { m_size.x, m_size.y });
		checkbox.Fill(Nz::Color::White, Nz::Rectui { m_borderSize.x, m_borderSize.y, m_size.x - (m_borderSize.x * 2), m_size.y - (m_borderSize.y * 2) });

		m_checkboxSprite->SetTexture(Nz::Texture::New(checkbox).Get());
		m_checkboxSprite->SetSize(static_cast<float>(m_size.x), static_cast<float>(m_size.y));

		Nz::String checkboxString;

		if (m_state == CheckboxState_Checked)
			checkboxString.Set(u8"✓"); // ✓

		if (m_state == CheckboxState_Tristate)
			checkboxString.Set(u8"■"); // ■

		m_checkboxTextSprite->Update(Nz::SimpleTextDrawer::Draw(checkboxString, (m_size.x + m_size.y) / 2u - (m_borderSize.x + m_borderSize.y) / 2u - 2u,
																0u, Nz::Color::Black));
	}
}
