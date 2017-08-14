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
	m_disabledBackgroundColor { 201, 201, 201 },
	m_checkedGlyphFlipped { false },
	m_tristateGlyphFlipped { false }
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

		Nz::Vector2f checkboxTextBox = m_checkboxContentSprite->GetBoundingVolume().obb.localBox.GetLengths(); // todo redo this

		if ((m_state == CheckboxState_Checked && m_checkedGlyphFlipped) || (m_state == CheckboxState_Tristate && m_tristateGlyphFlipped))
			m_checkboxContentEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x / 2.f - checkboxTextBox.y / 2.f,
		                                                                       origin.y + checkboxSize.y / 2.f - checkboxTextBox.x / 2.f);

		else
			m_checkboxContentEntity->GetComponent<NodeComponent>().SetPosition(origin.x + checkboxSize.x / 2.f - checkboxTextBox.x / 2.f,
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
			m_checkboxContentEntity->GetComponent<NodeComponent>().SetRotation(Nz::EulerAnglesf { 0.f, 0.f, m_checkedGlyphFlipped ? 90.f : 0.f });
			m_checkboxContentSprite->SetTexture(m_checkboxContentChecked, false);
			m_checkboxContentSprite->SetTextureRect(m_checkedRect);
		}

		else
		{
			m_checkboxContentEntity->Enable();
			m_checkboxContentEntity->GetComponent<NodeComponent>().SetRotation(Nz::EulerAnglesf { 0.f, 0.f, m_tristateGlyphFlipped ? 90.f : 0.f });
			m_checkboxContentSprite->SetTexture(m_checkboxContentTristate, false);
			m_checkboxContentSprite->SetTextureRect(m_tristateRect);
		}

		m_checkboxContentSprite->SetColor(m_checkboxEnabled ? m_mainColor : m_disabledMainColor);
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


		SetCheckboxSize(Nz::Vector2f { static_cast<float>(checkboxSize.x), static_cast<float>(checkboxSize.y) });
		RegenerateGlyphs();
	}

	void CheckboxWidget::RegenerateGlyphs(unsigned quality)
	{
		Nz::Vector2ui qualitySize = Nz::Vector2ui { static_cast<unsigned>(GetCheckboxSize().x), static_cast<unsigned>(GetCheckboxSize().y) } * quality;
		const unsigned charSize = (qualitySize.x + qualitySize.y) / 2;

		Nz::FontRef font = Nz::Font::GetDefault();
		const Nz::AbstractAtlas* atlas = font->GetAtlas().get();

		NazaraAssert(atlas, "Atlas is null");
		NazaraAssert(atlas->GetStorage() == Nz::DataStorage_Hardware, "Font uses a non-hardware atlas");

		Nz::Font::Glyph glyph;
		Nz::AbstractImage* glyphTexture {};


		glyph = font->GetGlyph(charSize, 0u, U'v'); // ✓

		if (!glyph.valid)
		{
			NazaraError("Failed to load checked UTF-8 glyph");
			glyph = font->GetGlyph(charSize, 0u, 'v');

			if (!glyph.valid)
			{
				NazaraError("Failed to load checked fallback glyph");
				NazaraAssert(false, "Cannot load any glyph");
			}
		}


		glyphTexture = atlas->GetLayer(glyph.layerIndex);
		NazaraAssert(glyphTexture, "Glyph Texture is null");

		m_checkedRect = glyph.atlasRect;
		m_checkedGlyphFlipped = glyph.flipped;
		m_checkboxContentChecked = static_cast<Nz::Texture*>(glyphTexture);
		NazaraAssert(m_checkboxContentChecked, "Checked texture is null");


		glyphTexture = nullptr;


		glyph = font->GetGlyph(charSize, 0u, U'◼'); // ◼

		if (!glyph.valid)
		{
			NazaraError("Failed to load tristate UTF-8 glyph");
			glyph = font->GetGlyph(charSize, 0u, 'o');

			if (!glyph.valid)
			{
				NazaraError("Failed to load tristate fallback glyph");
				NazaraAssert(false, "Cannot load any glyph");
			}
		}


		glyphTexture = atlas->GetLayer(glyph.layerIndex);
		NazaraAssert(glyphTexture, "Glyph Texture is null");

		m_tristateRect = glyph.atlasRect;
		m_tristateGlyphFlipped = glyph.flipped;
		m_checkboxContentTristate = static_cast<Nz::Texture*>(glyphTexture);
		NazaraAssert(m_checkboxContentTristate, "Checked texture is null");
	}
}
