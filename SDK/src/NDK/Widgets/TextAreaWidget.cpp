// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/World.hpp>
#include <limits>

namespace Ndk
{
	TextAreaWidget::TextAreaWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_cursorPosition(0U)
	{
		m_cursorSprite = Nz::Sprite::New();
		m_cursorSprite->SetColor(Nz::Color(192, 192, 192));
		m_cursorSprite->SetSize(1.f, m_drawer.GetFont()->GetSizeInfo(m_drawer.GetCharacterSize()).lineHeight);

		m_cursorEntity = CreateEntity();
		m_cursorEntity->AddComponent<GraphicsComponent>().Attach(m_cursorSprite, 10);
		m_cursorEntity->AddComponent<NodeComponent>().SetParent(this);
		m_cursorEntity->Enable(false);

		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);

		Layout();
	}

	void TextAreaWidget::AppendText(const Nz::String& text)
	{
		m_drawer.AppendText(text);

		m_textSprite->Update(m_drawer);
	}

	std::size_t TextAreaWidget::GetHoveredGlyph(float x, float y) const
	{
		std::size_t glyphCount = m_drawer.GetGlyphCount();
		if (glyphCount > 0)
		{
			std::size_t lineCount = m_drawer.GetLineCount();
			std::size_t line = 0U;
			for (; line < lineCount - 1; ++line)
			{
				Nz::Rectf lineBounds = m_drawer.GetLine(line).bounds;
				if (lineBounds.GetMaximum().y > y)
					break;
			}

			std::size_t upperLimit = (line != lineCount - 1) ? m_drawer.GetLine(line + 1).glyphIndex : glyphCount + 1;

			std::size_t i = m_drawer.GetLine(line).glyphIndex;
			for (; i < upperLimit - 1; ++i)
			{
				Nz::Rectf bounds = m_drawer.GetGlyph(i).bounds;
				if (x < bounds.x + bounds.width)
					break;
			}

			return i;
		}

		return 0;
	}

	void TextAreaWidget::ResizeToContent()
	{
		SetContentSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}

	void TextAreaWidget::SetText(const Nz::String& text)
	{
		m_drawer.SetText(text);

		m_textSprite->Update(m_drawer);
	}

	void TextAreaWidget::RefreshCursor()
	{
		std::size_t lineCount = m_drawer.GetLineCount();
		std::size_t line = 0U;
		for (std::size_t i = line + 1; i < lineCount; ++i)
		{
			if (m_drawer.GetLine(i).glyphIndex > m_cursorPosition)
				break;

			line = i;
		}

		const auto& lineInfo = m_drawer.GetLine(line);

		std::size_t glyphCount = m_drawer.GetGlyphCount();
		float position;
		if (glyphCount > 0 && lineInfo.glyphIndex < m_cursorPosition)
		{
			const auto& glyph = m_drawer.GetGlyph(std::min(m_cursorPosition, glyphCount - 1));
			position = glyph.bounds.x;
			if (m_cursorPosition >= glyphCount)
				position += glyph.bounds.width;
		}
		else
			position = 0.f;

		m_cursorEntity->GetComponent<Ndk::NodeComponent>().SetPosition(position, lineInfo.bounds.y);
	}

	void TextAreaWidget::OnMouseEnter()
	{
		m_cursorEntity->Enable(true);
	}

	void TextAreaWidget::OnMouseButtonPress(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			GrabKeyboard();

			m_cursorPosition = GetHoveredGlyph(x, y);
			RefreshCursor();
		}
	}

	void TextAreaWidget::OnMouseMoved(int x, int y, int deltaX, int deltaY)
	{
	}

	void TextAreaWidget::OnMouseExit()
	{
		m_cursorEntity->Enable(false);
	}

	void TextAreaWidget::OnTextEntered(char32_t character, bool /*repeated*/)
	{
		switch (character)
		{
			case '\b':
			{
				Nz::String text = m_drawer.GetText();

				text.Resize(-1, Nz::String::HandleUtf8);
				m_drawer.SetText(text);

				m_textSprite->Update(m_drawer);
				break;
			}

			case '\r':
			case '\n':
				Write(Nz::String('\n'));
				break;

			default:
			{
				if (Nz::Unicode::GetCategory(character) == Nz::Unicode::Category_Other_Control)
					break;

				Write(Nz::String::Unicode(character));
				break;
			}
		}
	}

	void TextAreaWidget::Write(const Nz::String& text)
	{
		if (m_cursorPosition >= m_drawer.GetGlyphCount())
		{
			AppendText(text);
			m_cursorPosition = m_drawer.GetGlyphCount();
		}
		else
		{
			Nz::String currentText = m_drawer.GetText();
			currentText.Insert(m_cursorPosition, text);
			SetText(currentText);

			m_cursorPosition += text.GetSize();
		}

		RefreshCursor();
	}
}
