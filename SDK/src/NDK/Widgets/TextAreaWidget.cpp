// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TextAreaWidget.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	TextAreaWidget::TextAreaWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_characterFilter(),
	m_echoMode(EchoMode_Normal),
	m_cursorPositionBegin(0U, 0U),
	m_cursorPositionEnd(0U, 0U),
	m_isMouseButtonDown(false),
	m_multiLineEnabled(false),
	m_readOnly(false),
	m_tabEnabled(false)
	{
		m_cursorEntity = CreateEntity(true);
		m_cursorEntity->AddComponent<GraphicsComponent>();
		m_cursorEntity->AddComponent<NodeComponent>().SetParent(this);
		m_cursorEntity->Enable(false);

		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity(true);
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);
		m_textEntity->AddComponent<NodeComponent>().SetParent(this);

		SetCursor(Nz::SystemCursor_Text);

		Layout();
	}

	void TextAreaWidget::AppendText(const Nz::String& text)
	{
		m_text += text;
		switch (m_echoMode)
		{
			case EchoMode_Normal:
				m_drawer.AppendText(text);
				break;

			case EchoMode_Password:
				m_drawer.AppendText(Nz::String(text.GetLength(), '*'));
				break;

			case EchoMode_PasswordExceptLast:
			{
				m_drawer.Clear();
				std::size_t textLength = m_text.GetLength();
				if (textLength >= 2)
				{
					std::size_t lastCharacterPosition = m_text.GetCharacterPosition(textLength - 2);
					if (lastCharacterPosition != Nz::String::npos)
						m_drawer.AppendText(Nz::String(textLength - 1, '*'));
				}

				if (textLength >= 1)
					m_drawer.AppendText(m_text.SubString(m_text.GetCharacterPosition(textLength - 1)));

				break;
			}
		}

		m_textSprite->Update(m_drawer);

		OnTextChanged(this, m_text);
	}

	void TextAreaWidget::Delete(std::size_t firstGlyph, std::size_t lastGlyph)
	{
		if (firstGlyph > lastGlyph)
			std::swap(firstGlyph, lastGlyph);

		std::size_t textLength = m_text.GetLength();
		if (firstGlyph > textLength)
			return;

		Nz::String newText;
		if (firstGlyph > 0)
			newText.Append(m_text.SubString(0, m_text.GetCharacterPosition(firstGlyph) - 1));

		if (lastGlyph < textLength)
			newText.Append(m_text.SubString(m_text.GetCharacterPosition(lastGlyph)));

		SetText(newText);
	}

	void TextAreaWidget::EraseSelection()
	{
		if (!HasSelection())
			return;

		Delete(GetGlyphIndex(m_cursorPositionBegin), GetGlyphIndex(m_cursorPositionEnd));
	}

	Nz::Vector2ui TextAreaWidget::GetHoveredGlyph(float x, float y) const
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

			std::size_t firstLineGlyph = m_drawer.GetLine(line).glyphIndex;
			std::size_t i = firstLineGlyph;
			for (; i < upperLimit - 1; ++i)
			{
				Nz::Rectf bounds = m_drawer.GetGlyph(i).bounds;
				if (x < bounds.x + bounds.width * 0.75f)
					break;
			}

			return Nz::Vector2ui(i - firstLineGlyph, line);
		}

		return Nz::Vector2ui::Zero();
	}

	void TextAreaWidget::ResizeToContent()
	{
		SetContentSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}

	void TextAreaWidget::Write(const Nz::String& text, std::size_t glyphPosition)
	{
		if (glyphPosition >= m_drawer.GetGlyphCount())
		{
			AppendText(text);
			SetCursorPosition(m_drawer.GetGlyphCount());
		}
		else
		{
			m_text.Insert(m_text.GetCharacterPosition(glyphPosition), text);
			SetText(m_text);

			SetCursorPosition(glyphPosition + text.GetLength());
		}
	}

	void TextAreaWidget::Layout()
	{
		BaseWidget::Layout();

		m_textEntity->GetComponent<NodeComponent>().SetPosition(GetContentOrigin());

		RefreshCursor();
	}

	bool TextAreaWidget::IsFocusable() const
	{
		return !m_readOnly;
	}

	void TextAreaWidget::OnFocusLost()
	{
		m_cursorEntity->Disable();
	}

	void TextAreaWidget::OnFocusReceived()
	{
		if (!m_readOnly)
			m_cursorEntity->Enable(true);
	}

	bool TextAreaWidget::OnKeyPressed(const Nz::WindowEvent::KeyEvent& key)
	{
		switch (key.code)
		{
			case Nz::Keyboard::Delete:
			{
				if (HasSelection())
					EraseSelection();
				else
					Delete(GetGlyphIndex(m_cursorPositionBegin));

				return true;
			}

			case Nz::Keyboard::Down:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyDown(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					SetCursorPosition(m_cursorPositionEnd);

				MoveCursor({0, 1});
				return true;
			}

			case Nz::Keyboard::End:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyEnd(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				std::size_t lineCount = m_drawer.GetLineCount();
				if (key.control && lineCount > 0)
					SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(lineCount - 1)), static_cast<unsigned int>(lineCount - 1) });
				else
					SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });

				return true;
			}

			case Nz::Keyboard::Home:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyHome(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				SetCursorPosition({ 0U, key.control ? 0U : m_cursorPositionEnd.y });
				return true;
			}

			case Nz::Keyboard::Left:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyLeft(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					SetCursorPosition(m_cursorPositionBegin);
				else if (key.control)
				{
					std::size_t index = GetGlyphIndex(m_cursorPositionBegin);

					if (index == 0)
						return true;

					std::size_t spaceIndex = m_text.FindLast(' ', index - 2);
					std::size_t endlIndex = m_text.FindLast('\n', index - 1);

					if ((spaceIndex > endlIndex || endlIndex == Nz::String::npos) && spaceIndex != Nz::String::npos)
						SetCursorPosition(spaceIndex + 1);
					else if (endlIndex != Nz::String::npos)
					{
						if (index == endlIndex + 1)
							SetCursorPosition(endlIndex);
						else
							SetCursorPosition(endlIndex + 1);
					}
					else
						SetCursorPosition({ 0U, m_cursorPositionBegin.y });
				}
				else
					MoveCursor(-1);

				return true;
			}

			case Nz::Keyboard::Right:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyRight(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					SetCursorPosition(m_cursorPositionEnd);
				else if (key.control)
				{
					std::size_t index = GetGlyphIndex(m_cursorPositionEnd);
					std::size_t spaceIndex = m_text.Find(' ', index);
					std::size_t endlIndex = m_text.Find('\n', index);

					if (spaceIndex < endlIndex && spaceIndex != Nz::String::npos)
					{
						if (m_text.GetSize() > spaceIndex)
							SetCursorPosition(spaceIndex + 1);
						else
							SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });
					}
					else if (endlIndex != Nz::String::npos)
					{
						if (index == endlIndex)
							SetCursorPosition(endlIndex + 1);
						else
							SetCursorPosition(endlIndex);
					}
					else
						SetCursorPosition({ static_cast<unsigned int>(m_drawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });
				}
				else
					MoveCursor(1);

				return true;
			}

			case Nz::Keyboard::Up:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyUp(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					SetCursorPosition(m_cursorPositionBegin);

				MoveCursor({0, -1});
				return true;
			}

			case Nz::Keyboard::Tab:
			{
				if (!m_tabEnabled)
					return false;

				if (HasSelection())
					EraseSelection();

				if (!key.shift)
					Write(Nz::String('\t'));
				else
				{
					std::size_t cursorGlyphEnd = GetGlyphIndex(m_cursorPositionEnd);

					if (cursorGlyphEnd > 0)
						if (m_text[cursorGlyphEnd - 1] == '\t')
							OnTextEntered('\b', false);
				}

				return true;
			}

			default:
				return false;
		}
	}

	void TextAreaWidget::OnKeyReleased(const Nz::WindowEvent::KeyEvent& /*key*/)
	{
	}

	void TextAreaWidget::OnMouseButtonPress(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			SetFocus();

			const Padding& padding = GetPadding();
			Nz::Vector2ui hoveredGlyph = GetHoveredGlyph(float(x - padding.left), float(y - padding.top));

			// Shift extends selection
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::RShift))
				SetSelection(hoveredGlyph, m_selectionCursor);
			else
			{
				SetCursorPosition(hoveredGlyph);
				m_selectionCursor = m_cursorPositionBegin;
			}

			m_isMouseButtonDown = true;
		}
	}

	void TextAreaWidget::OnMouseButtonRelease(int, int, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
			m_isMouseButtonDown = false;
	}

	void TextAreaWidget::OnMouseEnter()
	{
		if (!Nz::Mouse::IsButtonPressed(Nz::Mouse::Left))
			m_isMouseButtonDown = false;
	}

	void TextAreaWidget::OnMouseMoved(int x, int y, int deltaX, int deltaY)
	{
		if (m_isMouseButtonDown)
		{
			const Padding& padding = GetPadding();
			SetSelection(m_selectionCursor, GetHoveredGlyph(float(x - padding.left), float(y - padding.top)));
		}
	}

	void TextAreaWidget::OnTextEntered(char32_t character, bool /*repeated*/)
	{
		if (m_readOnly)
			return;

		switch (character)
		{
			case '\b':
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyBackspace(this, &ignoreDefaultAction);

				std::size_t cursorGlyphBegin = GetGlyphIndex(m_cursorPositionBegin);
				std::size_t cursorGlyphEnd = GetGlyphIndex(m_cursorPositionEnd);

				if (ignoreDefaultAction || cursorGlyphEnd == 0)
					break;

				// When a text is selected, delete key does the same as delete and leave the character behind it
				if (HasSelection())
					EraseSelection();
				else
				{
					Nz::String newText;

					if (cursorGlyphBegin > 1)
						newText.Append(m_text.SubString(0, m_text.GetCharacterPosition(cursorGlyphBegin - 1) - 1));

					if (cursorGlyphEnd < m_text.GetLength())
						newText.Append(m_text.SubString(m_text.GetCharacterPosition(cursorGlyphEnd)));

					// Move cursor before setting text (to prevent SetText to move our cursor)
					MoveCursor(-1);

					SetText(newText);
				}
				break;
			}

			case '\r':
			case '\n':
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyReturn(this, &ignoreDefaultAction);

				if (ignoreDefaultAction || !m_multiLineEnabled)
					break;

				if (HasSelection())
					EraseSelection();

				Write(Nz::String('\n'));
				break;
			}

			default:
			{
				if (Nz::Unicode::GetCategory(character) == Nz::Unicode::Category_Other_Control || (m_characterFilter && !m_characterFilter(character)))
					break;

				if (HasSelection())
					EraseSelection();

				Write(Nz::String::Unicode(character));
				break;
			}
		}
	}

	void TextAreaWidget::RefreshCursor()
	{
		if (m_readOnly)
			return;

		m_cursorEntity->GetComponent<NodeComponent>().SetPosition(GetContentOrigin());

		std::size_t selectionLineCount = m_cursorPositionEnd.y - m_cursorPositionBegin.y + 1;
		std::size_t oldSpriteCount = m_cursorSprites.size();
		if (m_cursorSprites.size() != selectionLineCount)
		{
			m_cursorSprites.resize(m_cursorPositionEnd.y - m_cursorPositionBegin.y + 1);
			for (std::size_t i = oldSpriteCount; i < m_cursorSprites.size(); ++i)
			{
				m_cursorSprites[i] = Nz::Sprite::New();
				m_cursorSprites[i]->SetMaterial(Nz::Material::New("Translucent2D"));
			}
		}

		float lineHeight = float(m_drawer.GetFont()->GetSizeInfo(m_drawer.GetCharacterSize()).lineHeight);

		GraphicsComponent& gfxComponent = m_cursorEntity->GetComponent<GraphicsComponent>();
		gfxComponent.Clear();

		for (unsigned int i = m_cursorPositionBegin.y; i <= m_cursorPositionEnd.y; ++i)
		{
			const auto& lineInfo = m_drawer.GetLine(i);

			Nz::SpriteRef& cursorSprite = m_cursorSprites[i - m_cursorPositionBegin.y];
			if (i == m_cursorPositionBegin.y || i == m_cursorPositionEnd.y)
			{
				auto GetGlyphPos = [&](unsigned int localGlyphPos)
				{
					std::size_t cursorGlyph = GetGlyphIndex({ localGlyphPos, i });

					std::size_t glyphCount = m_drawer.GetGlyphCount();
					float position;
					if (glyphCount > 0 && lineInfo.glyphIndex < cursorGlyph)
					{
						const auto& glyph = m_drawer.GetGlyph(std::min(cursorGlyph, glyphCount - 1));
						position = glyph.bounds.x;
						if (cursorGlyph >= glyphCount)
							position += glyph.bounds.width;
					}
					else
						position = 0.f;

					return position;
				};

				float beginX = (i == m_cursorPositionBegin.y) ? GetGlyphPos(m_cursorPositionBegin.x) : 0.f;
				float endX = (i == m_cursorPositionEnd.y) ? GetGlyphPos(m_cursorPositionEnd.x) : lineInfo.bounds.width;
				float spriteSize = std::max(endX - beginX, 1.f);

				cursorSprite->SetColor((m_cursorPositionBegin == m_cursorPositionEnd) ? Nz::Color::Black : Nz::Color(0, 0, 0, 50));
				cursorSprite->SetSize(spriteSize, float(m_drawer.GetFont()->GetSizeInfo(m_drawer.GetCharacterSize()).lineHeight));

				gfxComponent.Attach(cursorSprite, Nz::Matrix4f::Translate({ beginX, lineInfo.bounds.y, 0.f }));
			}
			else
			{
				cursorSprite->SetColor(Nz::Color(0, 0, 0, 50));
				cursorSprite->SetSize(lineInfo.bounds.width, float(m_drawer.GetFont()->GetSizeInfo(m_drawer.GetCharacterSize()).lineHeight));

				gfxComponent.Attach(cursorSprite, Nz::Matrix4f::Translate({ 0.f, lineInfo.bounds.y, 0.f }));
			}
		}
	}

	void TextAreaWidget::UpdateDisplayText()
	{
		switch (m_echoMode)
		{
			case EchoMode_Normal:
				m_drawer.SetText(m_text);
				break;

			case EchoMode_Password:
			case EchoMode_PasswordExceptLast:
				m_drawer.SetText(Nz::String(m_text.GetLength(), '*'));
				break;
		}

		m_textSprite->Update(m_drawer);

		SetCursorPosition(m_cursorPositionBegin); //< Refresh cursor position (prevent it from being outside of the text)
	}
}
