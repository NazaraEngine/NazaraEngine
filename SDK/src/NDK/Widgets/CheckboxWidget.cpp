// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/CheckboxWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <NDK/World.hpp>
#include <algorithm>

namespace Ndk
{
	CheckboxWidget::CheckboxWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_state { CheckboxState_Unchecked },
	m_tristateEnabled { false },
	m_checkboxEnabled { true },
	m_adaptativeMargin { true },
	m_textMargin { 16.f },
	m_borderScale { 8.f },
	m_mainColor { Nz::Color::Black },
	m_backgroundColor { Nz::Color::White },
	m_disabledMainColor { 62, 62, 62 },
	m_disabledBackgroundColor { 201, 201, 201 }
	{
		m_checkboxSprite = Nz::Sprite::New();
		m_checkboxSprite->SetMaterial(Nz::Material::New("Basic2D"));

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

		CreateCheckboxTextures();
		UpdateCheckboxSprite();
		Layout();
	}

	void CheckboxWidget::ResizeToContent()
	{
		Nz::Vector2f textSize = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		Nz::Vector2f checkboxSize = m_checkboxSprite->GetSize();

		Nz::Vector2f finalSize { checkboxSize.x + (m_adaptativeMargin ? checkboxSize.x / 2.f : m_textMargin) + textSize.x, std::max(textSize.y, checkboxSize.y) };
		SetContentSize(finalSize);
	}


	void CheckboxWidget::SetState(CheckboxState state)
	{
		if (!m_checkboxEnabled)
			return;

		if (state == CheckboxState_Tristate)
			m_tristateEnabled = true;

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
				SetState(m_tristateEnabled ? CheckboxState_Tristate : CheckboxState_Unchecked);
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
		Nz::Vector2f checkboxSize = GetCheckboxSize();
		Nz::Vector2f borderSize = GetCheckboxBorderSize();

		m_checkboxEntity->GetComponent<NodeComponent>().SetPosition(origin);

		Nz::Vector2f checkboxTextBox = m_checkboxTextSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_checkboxTextEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x / 2.f - checkboxTextBox.x / 2.f,
																		origin.y + checkboxSize.y / 2.f - checkboxTextBox.y / 2.f);
		
		Nz::Vector2f textBox = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_textEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x + (m_adaptativeMargin ? checkboxSize.x / 2.f : m_textMargin),
																origin.y + checkboxSize.y / 2.f - textBox.y / 2.f);
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
		if (m_checkboxEnabled)
			m_checkboxSprite->SetTexture(m_checkbox);

		else
			m_checkboxSprite->SetTexture(m_disabledCheckbox);

		Nz::String checkboxString;

		if (m_state == CheckboxState_Checked)
			checkboxString.Set(u8"v"); // ✓

		if (m_state == CheckboxState_Tristate)
			checkboxString.Set(u8"o"); // ■

		Nz::Vector2f checkboxSize = GetCheckboxSize();
		Nz::Vector2f borderSize = GetCheckboxBorderSize();

		m_checkboxTextSprite->Update(Nz::SimpleTextDrawer::Draw(checkboxString, static_cast<unsigned>((checkboxSize.x + checkboxSize.y) / 2.f - (borderSize.x + borderSize.y) / 2.f),
																0u, m_checkboxEnabled ? m_mainColor : m_disabledMainColor));
	}

	void CheckboxWidget::CreateCheckboxTextures()
	{
		Nz::Vector2ui checkboxSize = { 32u, 32u };
		Nz::Vector2ui borderSize = checkboxSize / static_cast<unsigned>(m_borderScale);


		Nz::Image checkbox;
		checkbox.Create(Nz::ImageType_2D, Nz::PixelFormatType_L8, checkboxSize.x, checkboxSize.y);

		checkbox.Fill(m_mainColor, Nz::Rectui { checkboxSize.x, checkboxSize.y });
		checkbox.Fill(m_backgroundColor, Nz::Rectui { borderSize.x, borderSize.y, checkboxSize.x - (borderSize.x * 2), checkboxSize.y - (borderSize.y * 2) });

		m_checkbox = Nz::Texture::New(checkbox);


		Nz::Image disabledCheckbox;
		disabledCheckbox.Create(Nz::ImageType_2D, Nz::PixelFormatType_L8, checkboxSize.x, checkboxSize.y);

		disabledCheckbox.Fill(m_disabledMainColor, Nz::Rectui { checkboxSize.x, checkboxSize.y });
		disabledCheckbox.Fill(m_disabledBackgroundColor, Nz::Rectui { borderSize.x, borderSize.y, checkboxSize.x - (borderSize.x * 2), checkboxSize.y - (borderSize.y * 2) });

		m_disabledCheckbox = Nz::Texture::New(disabledCheckbox);
	}
}
