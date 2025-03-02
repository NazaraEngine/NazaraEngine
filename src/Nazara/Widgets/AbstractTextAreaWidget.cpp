// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/AbstractTextAreaWidget.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>

namespace Nz
{
	AbstractTextAreaWidget::AbstractTextAreaWidget(BaseWidget* parent, const StyleFactory& styleFactory) :
	BaseWidget(parent),
	m_maximumTextLength(0),
	m_echoMode(EchoMode::Normal),
	m_textPadding(10.f, 10.f),
	m_cursorPositionBegin(0U, 0U),
	m_cursorPositionEnd(0U, 0U),
	m_isEnabled(true),
	m_isLineWrapEnabled(false),
	m_isMouseButtonDown(false),
	m_multiLineEnabled(false),
	m_readOnly(false),
	m_tabEnabled(false)
	{
		m_style = (styleFactory) ? styleFactory(this) : GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		SetCursor(SystemCursor::Text);
	}

	void AbstractTextAreaWidget::Clear()
	{
		AbstractTextDrawer& textDrawer = GetTextDrawer();
		textDrawer.Clear();
		UpdateTextSprite();

		m_cursorPositionBegin = Vector2ui::Zero();
		m_cursorPositionEnd = Vector2ui::Zero();

		RefreshCursorSize();
	}

	void AbstractTextAreaWidget::Enable(bool enable)
	{
		if (m_isEnabled == enable)
			return;

		if (enable)
		{
			EnableMouseInput(true);
			SetCursor(SystemCursor::Text);
			m_style->OnEnabled();
		}
		else
		{
			EnableMouseInput(false);
			SetCursor(SystemCursor::Default);
			m_style->OnDisabled();
		}

		m_isEnabled = enable;
	}

	void AbstractTextAreaWidget::EnableBackground(bool enable)
	{
		m_style->EnableBackground(enable);
	}

	void AbstractTextAreaWidget::EnableLineWrap(bool enable)
	{
		if (m_isLineWrapEnabled != enable)
		{
			m_isLineWrapEnabled = enable;

			AbstractTextDrawer& textDrawer = GetTextDrawer();

			if (enable)
				textDrawer.SetMaxLineWidth(GetWidth() - m_textPadding.x * 2.f);
			else
				textDrawer.SetMaxLineWidth(std::numeric_limits<float>::infinity());

			UpdateTextSprite();
		}
	}

	Vector2ui AbstractTextAreaWidget::GetHoveredGlyph(float x, float y) const
	{
		const AbstractTextDrawer& textDrawer = GetTextDrawer();

		x -= m_textOffset + m_textPadding.x;
		float textHeight = textDrawer.GetBounds().height;
		y = textHeight - y;

		std::size_t glyphCount = textDrawer.GetGlyphCount();
		const AbstractTextDrawer::Glyph* glyphs = textDrawer.GetGlyphs();
		if (glyphCount > 0)
		{
			std::size_t lineCount = textDrawer.GetLineCount();
			const AbstractTextDrawer::Line* lines = textDrawer.GetLines();
			std::size_t line = 0U;
			for (; line < lineCount - 1; ++line)
			{
				Rectf lineBounds = lines[line].bounds;
				if (lineBounds.GetMaximum().y > y)
					break;
			}

			std::size_t upperLimit = (line < lineCount - 1) ? lines[line + 1].glyphIndex : glyphCount + 1;

			std::size_t firstLineGlyph = lines[line].glyphIndex;
			std::size_t i = firstLineGlyph;
			for (; i < upperLimit - 1; ++i)
			{
				Rectf bounds = glyphs[i].bounds;
				if (x < bounds.x + bounds.width * 0.75f)
					break;
			}

			return Vector2ui(Vector2<std::size_t>(i - firstLineGlyph, line));
		}

		return Vector2ui::Zero();
	}

	void AbstractTextAreaWidget::SetBackgroundColor(const Color& color)
	{
		m_style->UpdateBackgroundColor(color);
	}

	void AbstractTextAreaWidget::SetMaximumTextLength(std::size_t maximumLength)
	{
		m_maximumTextLength = maximumLength;
	}

	bool AbstractTextAreaWidget::IsFocusable() const
	{
		return !m_readOnly && m_isEnabled;
	}

	void AbstractTextAreaWidget::Layout()
	{
		BaseWidget::Layout();

		if (m_isLineWrapEnabled)
		{
			AbstractTextDrawer& textDrawer = GetTextDrawer();
			textDrawer.SetMaxLineWidth(GetWidth() - m_textPadding.x * 2.f);
		}

		UpdateTextSprite();
		RefreshCursorSize();

		m_style->Layout(GetSize());
	}

	void AbstractTextAreaWidget::OnFocusLost()
	{
		m_style->OnFocusLost();
	}

	void AbstractTextAreaWidget::OnFocusReceived()
	{
		m_style->OnFocusReceived();
	}

	bool AbstractTextAreaWidget::OnKeyPressed(const WindowEvent::KeyEvent& key)
	{
		const AbstractTextDrawer& textDrawer = GetTextDrawer();

		switch (key.virtualKey)
		{
			// Select All (Ctrl+A)
			case Keyboard::VKey::A:
			{
				if (!key.control)
					break;

				SetSelection(Vector2ui::Zero(), Vector2ui(std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max()));
				return true;
			}

			// Copy (Ctrl+C)
			case Keyboard::VKey::C:
			{
				if (!key.control)
					break;

				bool ignoreDefaultAction = (m_echoMode != EchoMode::Normal);
				OnTextAreaKeyCopy(this, &ignoreDefaultAction);

				if (ignoreDefaultAction || !HasSelection())
					return true;

				CopySelectionToClipboard(m_cursorPositionBegin, m_cursorPositionEnd);
				return true;
			}

			// Paste (Ctrl+V)
			case Keyboard::VKey::V:
			{
				if (!key.control)
					break;

				bool ignoreDefaultAction = false;
				OnTextAreaKeyPaste(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					EraseSelection();

				PasteFromClipboard(m_cursorPositionBegin);
				return true;
			}

			case Keyboard::VKey::Insert:
			{
				if (key.control)
				{
					if (key.shift)
						break;

					// Copy (Ctrl + Insert)
					bool ignoreDefaultAction = (m_echoMode != EchoMode::Normal);
					OnTextAreaKeyCopy(this, &ignoreDefaultAction);

					if (ignoreDefaultAction || !HasSelection())
						return true;

					CopySelectionToClipboard(m_cursorPositionBegin, m_cursorPositionEnd);
					return true;
				}
				else if (key.shift)
				{
					// Paste (Shift + Insert)
					bool ignoreDefaultAction = false;
					OnTextAreaKeyPaste(this, &ignoreDefaultAction);

					if (ignoreDefaultAction)
						return true;

					if (HasSelection())
						EraseSelection();
				}
				return true;
			}

			// Cut (Ctrl+X)
			case Keyboard::VKey::X:
			{
				if (!key.control)
					break;

				bool ignoreDefaultAction = (m_echoMode != EchoMode::Normal);
				OnTextAreaKeyCut(this, &ignoreDefaultAction);

				if (ignoreDefaultAction || !HasSelection())
					return true;

				CopySelectionToClipboard(m_cursorPositionBegin, m_cursorPositionEnd);
				EraseSelection();

				return true;
			}

			case Keyboard::VKey::Backspace:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyBackspace(this, &ignoreDefaultAction);

				std::size_t cursorGlyphEnd = GetGlyphIndex(m_cursorPositionEnd);

				if (ignoreDefaultAction || cursorGlyphEnd == 0)
					return true;

				// When a text is selected, delete key does the same as delete and leave the character behind it
				if (HasSelection())
					EraseSelection();
				else
				{
					MoveCursor(-1);
					Erase(GetGlyphIndex(m_cursorPositionBegin));
				}

				return true;
			}

			case Keyboard::VKey::Delete:
			{
				if (HasSelection())
				{
					// Cut (Shift + Delete)
					if (key.shift)
					{
						bool ignoreDefaultAction = (m_echoMode != EchoMode::Normal);
						OnTextAreaKeyCut(this, &ignoreDefaultAction);

						if (ignoreDefaultAction || !HasSelection())
							return true;

						CopySelectionToClipboard(m_cursorPositionBegin, m_cursorPositionEnd);
					}

					EraseSelection();
				}
				else
					Erase(GetGlyphIndex(m_cursorPositionBegin));

				return true;
			}

			case Keyboard::VKey::Down:
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

			case Keyboard::VKey::End:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyEnd(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				std::size_t lineCount = textDrawer.GetLineCount();
				if (key.control && lineCount > 0)
					SetCursorPosition({ static_cast<unsigned int>(textDrawer.GetLineGlyphCount(lineCount - 1)), static_cast<unsigned int>(lineCount - 1) });
				else
					SetCursorPosition({ static_cast<unsigned int>(textDrawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });

				return true;
			}

			case Keyboard::VKey::Home:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyHome(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				SetCursorPosition({ 0U, key.control ? 0U : m_cursorPositionEnd.y });
				return true;
			}

			case Keyboard::VKey::Left:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyLeft(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					SetCursorPosition(m_cursorPositionBegin);
				else if (key.control)
					HandleWordCursorMove(true);
				else
					MoveCursor(-1);

				return true;
			}

			case Keyboard::VKey::Return:
			case Keyboard::VKey::NumpadReturn:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyReturn(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (!m_multiLineEnabled)
					break;

				if (HasSelection())
					EraseSelection();

				Write("\n");
				return true;
			}

			case Keyboard::VKey::Right:
			{
				bool ignoreDefaultAction = false;
				OnTextAreaKeyRight(this, &ignoreDefaultAction);

				if (ignoreDefaultAction)
					return true;

				if (HasSelection())
					SetCursorPosition(m_cursorPositionEnd);
				else if (key.control)
					HandleWordCursorMove(false);
				else
					MoveCursor(1);

				return true;
			}

			case Keyboard::VKey::Up:
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

			case Keyboard::VKey::Tab:
			{
				if (!m_tabEnabled)
					return false;

				if (HasSelection())
					HandleSelectionIndentation(!key.shift);
				else
					HandleIndentation(!key.shift);

				return true;
			}

			default:
				break;
		}

		return false;
	}

	bool AbstractTextAreaWidget::OnMouseButtonDoublePress(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			// Shift double click is handled as single click
			if (Keyboard::IsKeyPressed(Keyboard::VKey::LShift) || Keyboard::IsKeyPressed(Keyboard::VKey::RShift))
				return OnMouseButtonPress(x, y, button);

			SetFocus();

			Vector2ui hoveredGlyph = GetHoveredGlyph(float(x), float(y));
			HandleWordSelection(hoveredGlyph);

			m_isMouseButtonDown = true;
			return true;
		}

		return false;
	}

	bool AbstractTextAreaWidget::OnMouseButtonPress(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			SetFocus();

			Vector2ui hoveredGlyph = GetHoveredGlyph(float(x), float(y));

			// Shift extends selection
			if (Keyboard::IsKeyPressed(Keyboard::VKey::LShift) || Keyboard::IsKeyPressed(Keyboard::VKey::RShift))
				SetSelection(hoveredGlyph, m_selectionCursor);
			else
			{
				SetCursorPosition(hoveredGlyph);
				m_selectionCursor = m_cursorPositionBegin;
			}

			m_isMouseButtonDown = true;
			return true;
		}

		return false;
	}

	bool AbstractTextAreaWidget::OnMouseButtonRelease(int, int, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			m_isMouseButtonDown = false;
			return true;
		}

		return false;
	}

	bool AbstractTextAreaWidget::OnMouseButtonTriplePress(int x, int y, Mouse::Button button)
	{
		if (button == Mouse::Left)
		{
			// Shift triple click is handled as single click
			if (Keyboard::IsKeyPressed(Keyboard::VKey::LShift) || Keyboard::IsKeyPressed(Keyboard::VKey::RShift))
				return OnMouseButtonPress(x, y, button);

			SetFocus();

			Vector2ui hoveredGlyph = GetHoveredGlyph(float(x), float(y));
			SetSelection(Vector2ui(0, hoveredGlyph.y), Vector2ui(std::numeric_limits<unsigned int>::max(), hoveredGlyph.y));

			m_isMouseButtonDown = true;
			return true;
		}

		return false;
	}

	void AbstractTextAreaWidget::OnMouseEnter()
	{
		if (!Mouse::IsButtonPressed(Mouse::Left))
			m_isMouseButtonDown = false;
	}

	bool AbstractTextAreaWidget::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
	{
		if (m_isMouseButtonDown)
		{
			SetSelection(m_selectionCursor, GetHoveredGlyph(float(x), float(y)));
			return true;
		}

		return false;
	}

	void AbstractTextAreaWidget::OnRenderLayerUpdated(int baseRenderLayer)
	{
		m_style->UpdateRenderLayer(baseRenderLayer);
	}

	bool AbstractTextAreaWidget::OnTextEntered(char32_t character, bool /*repeated*/)
	{
		if (!m_isEnabled || m_readOnly)
			return false;

		if (Unicode::GetCategory(character) == Unicode::Category_Other_Control || (m_characterFilter && !m_characterFilter(character)))
			return false;

		if (HasSelection())
			EraseSelection();

		Write(FromUtf32String(std::u32string_view(&character, 1)));
		return true;
	}

	void AbstractTextAreaWidget::RefreshCursorSize()
	{
		if (!m_isEnabled || m_readOnly)
			return;

		const AbstractTextDrawer& textDrawer = GetTextDrawer();

		const AbstractTextDrawer::Glyph* glyphs = textDrawer.GetGlyphs();
		const AbstractTextDrawer::Line* lines = textDrawer.GetLines();
		std::size_t lineCount = textDrawer.GetLineCount();

		auto GetGlyph = [&](const Vector2ui& glyphPosition) -> std::pair<const AbstractTextDrawer::Glyph*, bool>
		{
			if (glyphPosition.y >= lineCount)
				return { nullptr, true };

			const auto& lineInfo = lines[glyphPosition.y];

			std::size_t cursorGlyph = GetGlyphIndex({ glyphPosition.x, glyphPosition.y });

			std::size_t glyphCount = textDrawer.GetGlyphCount();
			if (glyphCount > 0 && lineInfo.glyphIndex < cursorGlyph)
			{
				const auto& glyph = glyphs[std::min(cursorGlyph, glyphCount - 1)];
				// We are overshooting if we are past the glyph count on our line
				std::size_t lineGlyphLimit = (glyphPosition.y < lineCount - 1) ? lines[glyphPosition.y + 1].glyphIndex - 1 : glyphCount;
				bool overshooting = (cursorGlyph >= lineGlyphLimit) && lines[glyphPosition.y].allowsOvershoot;
				return { &glyph, overshooting };
			}
			else
				return { nullptr, true };
		};

		// Move text so that cursor always lies in drawer bounds
		auto [lastGlyph, overshooting] = GetGlyph(m_cursorPositionEnd);
		float glyphPos = (lastGlyph) ? lastGlyph->bounds.x : 0.f;
		float glyphWidth = (lastGlyph) ? lastGlyph->bounds.width : 0.f;
		float textPosition = m_textOffset;
		float width = GetWidth() - m_textPadding.x;

		float cursorPosition = glyphPos + textPosition + ((overshooting) ? glyphWidth : 0.f);

		if (width <= textDrawer.GetBounds().width)
		{
			if (cursorPosition + glyphWidth > width)
				UpdateTextOffset(m_textOffset + width - cursorPosition - glyphWidth);
			else if (cursorPosition - glyphWidth < 0.f)
				UpdateTextOffset(m_textOffset - cursorPosition + glyphWidth);
		}
		else
			UpdateTextOffset(0.f); //< Reset text position if we have enough room to show everything

		// Create/destroy cursor entities and sprites

		std::size_t selectionLineCount = m_cursorPositionEnd.y - m_cursorPositionBegin.y + 1;
		m_cursorRects.clear();
		m_cursorRects.reserve(selectionLineCount);

		// Resize every cursor sprite
		float textHeight = GetTextDrawer().GetBounds().height;
		for (unsigned int i = m_cursorPositionBegin.y; i <= m_cursorPositionEnd.y; ++i)
		{
			const auto& lineInfo = lines[i];

			if (i == m_cursorPositionBegin.y || i == m_cursorPositionEnd.y)
			{
				// Partial selection (or no selection)
				auto GetGlyphPos = [&](const Vector2ui& glyphPosition)
				{
					auto [glyph, overshooting] = GetGlyph(glyphPosition);
					if (glyph)
					{
						float position = glyph->bounds.x;
						if (overshooting)
							position += glyph->bounds.width;

						return position;
					}
					else
						return 0.f;
				};

				float beginX = (i == m_cursorPositionBegin.y) ? GetGlyphPos({ m_cursorPositionBegin.x, i }) : 0.f;
				float endX = (i == m_cursorPositionEnd.y) ? GetGlyphPos({ m_cursorPositionEnd.x, i }) : lineInfo.bounds.width;
				float spriteSize = std::max(endX - beginX, 1.f);

				auto& cursorSize = m_cursorRects.emplace_back();
				cursorSize.x = beginX;
				cursorSize.y = textHeight - lineInfo.bounds.y - lineInfo.bounds.height;
				cursorSize.width = spriteSize;
				cursorSize.height = lineInfo.bounds.height;
			}
			else
			{
				// Full line selection
				auto& cursorSize = m_cursorRects.emplace_back();
				cursorSize.x = 0.f;
				cursorSize.y = textHeight - lineInfo.bounds.y - lineInfo.bounds.height;
				cursorSize.width = lineInfo.bounds.width;
				cursorSize.height = lineInfo.bounds.height;
			}
		}

		m_style->UpdateCursors(m_cursorRects);
	}

	void AbstractTextAreaWidget::UpdateTextOffset(float offset)
	{
		m_style->UpdateTextOffset(offset);
		m_textOffset = offset;
	}

	void AbstractTextAreaWidget::UpdateTextSprite()
	{
		const AbstractTextDrawer& textDrawer = GetTextDrawer();
		m_style->UpdateText(textDrawer);

		const AbstractTextDrawer::Line* lines = textDrawer.GetLines();

		float preferredHeight = 0.f;
		std::size_t lineCount = textDrawer.GetLineCount();
		for (std::size_t i = 0; i < lineCount; ++i)
			preferredHeight += lines[i].bounds.height;

		SetPreferredSize({ -1.f, preferredHeight + m_textPadding.y * 2.f });
	}
}
