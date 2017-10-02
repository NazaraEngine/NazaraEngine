// Copyright (C) 2017 Samy Bensaid
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/CheckboxWidget.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <algorithm>

namespace Ndk
{
	Nz::Color CheckboxWidget::s_backgroundColor { Nz::Color::White };
	Nz::Color CheckboxWidget::s_disabledBackgroundColor { 201, 201, 201 };
	Nz::Color CheckboxWidget::s_disabledBorderColor { 62, 62, 62 };
	Nz::Color CheckboxWidget::s_borderColor { Nz::Color::Black };
	float CheckboxWidget::s_borderScale { 16.f };

	CheckboxWidget::CheckboxWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_adaptativeMargin { true },
	m_checkboxEnabled { true },
	m_tristateEnabled { false },
	m_textMargin { 16.f },
	m_state	{ CheckboxState_Unchecked }
	{
		m_checkboxBorderSprite = Nz::Sprite::New(Nz::Material::New("Basic2D"));
		m_checkboxBackgroundSprite = Nz::Sprite::New(Nz::Material::New("Basic2D"));
		m_checkboxContentSprite = Nz::Sprite::New(Nz::Material::New("Translucent2D"));
		m_textSprite = Nz::TextSprite::New();

		m_checkboxBorderEntity = CreateEntity();
		m_checkboxBorderEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxBorderEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxBorderSprite);

		m_checkboxBackgroundEntity = CreateEntity();
		m_checkboxBackgroundEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxBackgroundEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxBackgroundSprite, 1);

		m_checkboxContentEntity = CreateEntity();
		m_checkboxContentEntity->AddComponent<NodeComponent>().SetParent(this);
		m_checkboxContentEntity->AddComponent<GraphicsComponent>().Attach(m_checkboxContentSprite, 2);

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);

		m_checkMark = Nz::TextureLibrary::Get("Ndk::CheckboxWidget::checkmark");

		SetCheckboxSize({ 32.f, 32.f });
		UpdateCheckbox();
	}

	bool CheckboxWidget::Initialize()
	{
		const Nz::UInt8 r_checkmark[] =
		{
			#include <NDK/Resources/checkmark.png.h>
		};

		Nz::TextureRef checkmarkTexture = Nz::Texture::New();
		if (!checkmarkTexture->LoadFromMemory(r_checkmark, sizeof(r_checkmark) / sizeof(r_checkmark[0])))
		{
			NazaraError("Failed to load embedded checkmark");
			return false;
		}

		Nz::TextureLibrary::Register("Ndk::CheckboxWidget::checkmark", checkmarkTexture);
		return true;
	}
	
	void CheckboxWidget::Uninitialize()
	{
		Nz::TextureLibrary::Unregister("Ndk::CheckboxWidget::checkmark");
	}

	void CheckboxWidget::SetState(CheckboxState state)
	{
		if (!m_checkboxEnabled)
			return;

		if (state == CheckboxState_Tristate)
			m_tristateEnabled = true;

		m_state = state;
		UpdateCheckbox();
	}

	CheckboxState CheckboxWidget::SwitchToNextState()
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
		Nz::Vector2f checkboxSize = GetCheckboxSize();

		Nz::Vector2f finalSize { checkboxSize.x + (m_adaptativeMargin ? checkboxSize.x / 2.f : m_textMargin) + textSize.x, std::max(textSize.y, checkboxSize.y) };
		SetContentSize(finalSize);
	}

	void CheckboxWidget::Layout()
	{
		BaseWidget::Layout();

		Nz::Vector2f origin = GetContentOrigin();
		Nz::Vector2f checkboxSize = GetCheckboxSize();
		Nz::Vector2f borderSize = GetCheckboxBorderSize();

		m_checkboxBorderEntity->GetComponent<NodeComponent>().SetPosition(origin);
		m_checkboxBackgroundEntity->GetComponent<NodeComponent>().SetPosition(origin + borderSize);

		Nz::Vector3f checkboxBox = m_checkboxContentSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_checkboxContentEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x / 2.f - checkboxBox.x / 2.f,
		                                                                   origin.y + checkboxSize.y / 2.f - checkboxBox.y / 2.f);

		Nz::Vector3f textBox = m_textSprite->GetBoundingVolume().obb.localBox.GetLengths();
		m_textEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x + (m_adaptativeMargin ? checkboxSize.x / 2.f : m_textMargin),
																origin.y + checkboxSize.y / 2.f - textBox.y / 2.f);
	}

	void CheckboxWidget::OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left && ContainsCheckbox(x, y) && IsCheckboxEnabled())
		{
			SwitchToNextState();
			OnStateChanged(this);
		}
	}

	void CheckboxWidget::UpdateCheckbox()
	{
		if (m_checkboxEnabled)
		{
			m_checkboxBorderSprite->SetColor(s_borderColor);
			m_checkboxBackgroundSprite->SetColor(s_backgroundColor);
		}
		else
		{
			m_checkboxBorderSprite->SetColor(s_disabledBorderColor);
			m_checkboxBackgroundSprite->SetColor(s_disabledBackgroundColor);
		}


		if (m_state == CheckboxState_Unchecked)
		{
			m_checkboxContentEntity->Enable(false);
			return;
		}
		else if (m_state == CheckboxState_Checked)
		{
			m_checkboxContentEntity->Enable();
			m_checkboxContentSprite->SetColor(Nz::Color::White);
			m_checkboxContentSprite->SetTexture(m_checkMark, false);
		}
		else // Tristate
		{
			m_checkboxContentEntity->Enable();
			m_checkboxContentSprite->SetColor(Nz::Color::Black);
			m_checkboxContentSprite->SetTexture(Nz::TextureRef {});
		}
	}
}
