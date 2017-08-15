// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>
#include <NDK/Widgets/CheckboxWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/FontGlyph.hpp>
#include <algorithm>

namespace Ndk
{
	Nz::Color CheckboxWidget::s_backgroundColor { Nz::Color::White };
	Nz::Color CheckboxWidget::s_disabledBackgroundColor { 201, 201, 201 };
	Nz::Color CheckboxWidget::s_disabledMainColor { 62, 62, 62 };
	Nz::Color CheckboxWidget::s_mainColor { Nz::Color::Black };
	float CheckboxWidget::s_borderScale { 8.f };

	CheckboxWidget::CheckboxWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_adaptativeMargin { true },
	m_checkboxEnabled { true },
	m_tristateEnabled { false },
	m_textMargin { 16.f },
	m_state	{ CheckboxState_Unchecked }
	{
		m_checkboxSprite = Nz::Sprite::New(Nz::Material::New("Basic2D"));
		m_checkboxContentSprite = Nz::Sprite::New(Nz::Material::New("Translucent2D"));
		m_textSprite = Nz::TextSprite::New();

		m_checkboxEntity = CreateEntity();
		m_checkboxEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxSprite);

		m_checkboxContentEntity = CreateEntity();
		m_checkboxContentEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxContentEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxContentSprite, 1);

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);

		InitializeCheckboxTextures();
		SetCheckboxSize({ 32.f, 32.f });
		UpdateCheckboxSprite();
		Layout();
	}

	bool CheckboxWidget::Initialize()
	{
		Nz::Vector2ui checkboxSize = { 32u, 32u };
		Nz::Vector2ui borderSize = checkboxSize / static_cast<unsigned>(s_borderScale);


		Nz::Image checkbox;
		if (!checkbox.Create(Nz::ImageType_2D, Nz::PixelFormatType_L8, checkboxSize.x, checkboxSize.y)) return false;

		if (!checkbox.Fill(s_mainColor, Nz::Rectui { checkboxSize.x, checkboxSize.y })) return false;
		if (!checkbox.Fill(s_backgroundColor, Nz::Rectui { borderSize.x, borderSize.y, checkboxSize.x - (borderSize.x * 2), checkboxSize.y - (borderSize.y * 2) })) return false;

		Nz::TextureRef checkboxTexture = Nz::Texture::New();
		if (!checkboxTexture->LoadFromImage(checkbox)) return false;

		Nz::Image disabledCheckbox;
		if (!disabledCheckbox.Create(Nz::ImageType_2D, Nz::PixelFormatType_L8, checkboxSize.x, checkboxSize.y)) return false;

		if (!disabledCheckbox.Fill(s_disabledMainColor, Nz::Rectui { checkboxSize.x, checkboxSize.y })) return false;
		if (!disabledCheckbox.Fill(s_disabledBackgroundColor, Nz::Rectui { borderSize.x, borderSize.y, checkboxSize.x - (borderSize.x * 2), checkboxSize.y - (borderSize.y * 2) })) return false;

		Nz::TextureRef disabledCheckboxTexture = Nz::Texture::New();
		if (!disabledCheckboxTexture->LoadFromImage(disabledCheckbox)) return false;


		Nz::Image tristate;
		if (!tristate.Create(Nz::ImageType_2D, Nz::PixelFormatType_L8, 1u, 1u)) return false;
		if (!tristate.Fill(Nz::Color::Black)) return false;
		Nz::TextureRef tristateTexture = Nz::Texture::New();
		if (!tristateTexture->LoadFromImage(tristate)) return false;


		const Nz::UInt8 r_checkmark[] =
		{
			#include <NDK/Resources/checkmark.png.h>
		};

		Nz::TextureRef checkedTexture = Nz::Texture::New();
		if (!checkedTexture->LoadFromMemory(r_checkmark, sizeof(r_checkmark) / sizeof(r_checkmark[0])))
		{
			NazaraError("Failed to load embedded checkmark");
			return false;
		}

		Nz::TextureLibrary::Register("Ndk::CheckboxWidget::checkbox", checkboxTexture);
		Nz::TextureLibrary::Register("Ndk::CheckboxWidget::disabledCheckbox", disabledCheckboxTexture);
		Nz::TextureLibrary::Register("Ndk::CheckboxWidget::tristate", tristateTexture);
		Nz::TextureLibrary::Register("Ndk::CheckboxWidget::checked", checkedTexture);

		return true;
	}
	
	void CheckboxWidget::Uninitialize()
	{
		Nz::TextureLibrary::Unregister("Ndk::CheckboxWidget::checkbox");
		Nz::TextureLibrary::Unregister("Ndk::CheckboxWidget::disabledCheckbox");
		Nz::TextureLibrary::Unregister("Ndk::CheckboxWidget::tristate");
		Nz::TextureLibrary::Unregister("Ndk::CheckboxWidget::checked");
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


	void CheckboxWidget::ResizeToContent()
	{
		Nz::Vector3f textSize = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		Nz::Vector2f checkboxSize = m_checkboxSprite->GetSize();

		Nz::Vector2f finalSize { checkboxSize.x + (m_adaptativeMargin ? checkboxSize.x / 2.f : m_textMargin) + textSize.x, std::max(textSize.y, checkboxSize.y) };
		SetContentSize(finalSize);
	}


	void CheckboxWidget::Layout()
	{
		BaseWidget::Layout();

		Nz::Vector2f origin = GetContentOrigin();
		Nz::Vector2f checkboxSize = GetCheckboxSize();
		Nz::Vector2f borderSize = GetCheckboxBorderSize();

		m_checkboxEntity->GetComponent<NodeComponent>().SetPosition(origin);

		Nz::Vector3f checkboxTextBox = m_checkboxContentSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_checkboxContentEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x / 2.f - checkboxTextBox.x / 2.f,
		                                                                   origin.y + checkboxSize.y / 2.f - checkboxTextBox.y / 2.f);

		Nz::Vector3f textBox = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_textEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x + (m_adaptativeMargin ? checkboxSize.x / 2.f : m_textMargin),
																origin.y + checkboxSize.y / 2.f - textBox.y / 2.f);
	}

	void CheckboxWidget::OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left && ContainsCheckbox(x, y) && IsCheckboxEnabled())
		{
			SetNextState();
			OnStateChanged(this);
		}
	}


	void CheckboxWidget::UpdateCheckboxSprite()
	{
		if (m_checkboxEnabled)
			m_checkboxSprite->SetTexture(m_checkbox, false);
		else
			m_checkboxSprite->SetTexture(m_disabledCheckbox, false);


		if (m_state == CheckboxState_Unchecked)
		{
			m_checkboxContentEntity->Enable(false);
			return;
		}
		else if (m_state == CheckboxState_Checked)
		{
			m_checkboxContentEntity->Enable();
			m_checkboxContentSprite->SetTexture(m_checkboxContentChecked, false);
		}
		else
		{
			m_checkboxContentEntity->Enable();
			m_checkboxContentSprite->SetTexture(m_checkboxContentTristate, false);
		}

		m_checkboxContentSprite->SetColor(m_checkboxEnabled ? s_mainColor : s_disabledMainColor);
	}


	void CheckboxWidget::InitializeCheckboxTextures()
	{
		m_checkbox =  Nz::TextureLibrary::Get("Ndk::CheckboxWidget::checkbox");
		m_disabledCheckbox = Nz::TextureLibrary::Get("Ndk::CheckboxWidget::disabledCheckbox");
		m_checkboxContentTristate = Nz::TextureLibrary::Get("Ndk::CheckboxWidget::tristate");
		m_checkboxContentChecked = Nz::TextureLibrary::Get("Ndk::CheckboxWidget::checked");
	}
}
