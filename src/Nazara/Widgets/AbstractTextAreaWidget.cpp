// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Widgets/AbstractTextAreaWidget.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Graphics/Components/GraphicsComponent.hpp>
#include <Nazara/Utility/Font.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Widgets/Canvas.hpp>
#include <Nazara/Widgets/Widgets.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr float paddingWidth = 5.f;
		constexpr float paddingHeight = 3.f;
	}

	AbstractTextAreaWidget::AbstractTextAreaWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_characterFilter(),
	m_echoMode(EchoMode::Normal),
	m_cursorPositionBegin(0U, 0U),
	m_cursorPositionEnd(0U, 0U),
	m_isLineWrapEnabled(false),
	m_isMouseButtonDown(false),
	m_multiLineEnabled(false),
	m_readOnly(false),
	m_tabEnabled(false)
	{
		m_textSprite = std::make_shared<TextSprite>(Widgets::Instance()->GetTransparentMaterial());

		auto& registry = GetRegistry();

		m_textEntity = CreateEntity();

		auto& gfxComponent = registry.emplace<GraphicsComponent>(m_textEntity, IsVisible());
		gfxComponent.AttachRenderable(m_textSprite, GetCanvas()->GetRenderMask());

		auto& textNode = GetRegistry().emplace<NodeComponent>(m_textEntity);
		textNode.SetParent(this);
		textNode.SetPosition(paddingWidth, GetHeight() - paddingHeight);

		SetCursor(SystemCursor::Text);

		EnableBackground(true);
	}
	
	void AbstractTextAreaWidget::Clear()
	{
		AbstractTextDrawer& textDrawer = GetTextDrawer();
		textDrawer.Clear();
		UpdateTextSprite();

		m_cursorPositionBegin.MakeZero();
		m_cursorPositionEnd.MakeZero();

		RefreshCursor();
	}

	void AbstractTextAreaWidget::EnableLineWrap(bool enable)
	{
		if (m_isLineWrapEnabled != enable)
		{
			m_isLineWrapEnabled = enable;

			AbstractTextDrawer& textDrawer = GetTextDrawer();

			if (enable)
				textDrawer.SetMaxLineWidth(GetWidth());
			else
				textDrawer.SetMaxLineWidth(std::numeric_limits<float>::infinity());

			UpdateTextSprite();
		}
	}

	Vector2ui AbstractTextAreaWidget::GetHoveredGlyph(float x, float y) const
	{
		const AbstractTextDrawer& textDrawer = GetTextDrawer();

		auto& textNode = GetRegistry().get<NodeComponent>(m_textEntity);
		Vector2f textPosition = Vector2f(textNode.GetPosition(CoordSys::Local));
		x -= textPosition.x;
		y -= textPosition.y;

		float textHeight = textDrawer.GetBounds().height;
		y = textHeight - y;

		std::size_t glyphCount = textDrawer.GetGlyphCount();
		if (glyphCount > 0)
		{
			std::size_t lineCount = textDrawer.GetLineCount();
			std::size_t line = 0U;
			for (; line < lineCount - 1; ++line)
			{
				Rectf lineBounds = textDrawer.GetLine(line).bounds;
				if (lineBounds.GetMaximum().y > y)
					break;
			}

			std::size_t upperLimit = (line != lineCount - 1) ? textDrawer.GetLine(line + 1).glyphIndex : glyphCount + 1;

			std::size_t firstLineGlyph = textDrawer.GetLine(line).glyphIndex;
			std::size_t i = firstLineGlyph;
			for (; i < upperLimit - 1; ++i)
			{
				Rectf bounds = textDrawer.GetGlyph(i).bounds;
				if (x < bounds.x + bounds.width * 0.75f)
					break;
			}

			return Vector2ui(Vector2<std::size_t>(i - firstLineGlyph, line));
		}

		return Vector2ui::Zero();
	}

	bool AbstractTextAreaWidget::IsFocusable() const
	{
		return !m_readOnly;
	}

	void AbstractTextAreaWidget::Layout()
	{
		BaseWidget::Layout();

		if (m_isLineWrapEnabled)
		{
			AbstractTextDrawer& textDrawer = GetTextDrawer();

			textDrawer.SetMaxLineWidth(GetWidth());
			UpdateTextSprite();
		}

		RefreshCursor();
	}

	void AbstractTextAreaWidget::OnFocusLost()
	{
		// Hide cursors
		auto& registry = GetRegistry();
		for (auto& cursor : m_cursors)
			registry.get<GraphicsComponent>(cursor.entity).Hide();
	}

	void AbstractTextAreaWidget::OnFocusReceived()
	{
		if (!m_readOnly)
		{
			// Show cursors
			auto& registry = GetRegistry();
			for (auto& cursor : m_cursors)
				registry.get<GraphicsComponent>(cursor.entity).Show();
		}
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
					EraseSelection();
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

	void AbstractTextAreaWidget::OnKeyReleased(const WindowEvent::KeyEvent& /*key*/)
	{
	}

	void AbstractTextAreaWidget::OnMouseButtonDoublePress(int x, int y, Mouse::Button button)
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
		}
	}

	void AbstractTextAreaWidget::OnMouseButtonPress(int x, int y, Mouse::Button button)
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
		}
	}

	void AbstractTextAreaWidget::OnMouseButtonRelease(int, int, Mouse::Button button)
	{
		if (button == Mouse::Left)
			m_isMouseButtonDown = false;
	}

	void AbstractTextAreaWidget::OnMouseButtonTriplePress(int x, int y, Mouse::Button button)
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
		}
	}

	void AbstractTextAreaWidget::OnMouseEnter()
	{
		if (!Mouse::IsButtonPressed(Mouse::Left))
			m_isMouseButtonDown = false;
	}

	void AbstractTextAreaWidget::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
	{
		if (m_isMouseButtonDown)
			SetSelection(m_selectionCursor, GetHoveredGlyph(float(x), float(y)));
	}

	void AbstractTextAreaWidget::OnTextEntered(char32_t character, bool /*repeated*/)
	{
		if (m_readOnly)
			return;

		if (Unicode::GetCategory(character) == Unicode::Category_Other_Control || (m_characterFilter && !m_characterFilter(character)))
			return;

		if (HasSelection())
			EraseSelection();

		Write(FromUtf32String(std::u32string_view(&character, 1)));
	}

	void AbstractTextAreaWidget::RefreshCursor()
	{
		if (m_readOnly)
			return;

		const AbstractTextDrawer& textDrawer = GetTextDrawer();

		auto GetGlyph = [&](const Vector2ui& glyphPosition, std::size_t* glyphIndex) -> const AbstractTextDrawer::Glyph*
		{
			if (glyphPosition.y >= textDrawer.GetLineCount())
				return nullptr;

			const auto& lineInfo = textDrawer.GetLine(glyphPosition.y);

			std::size_t cursorGlyph = GetGlyphIndex({ glyphPosition.x, glyphPosition.y });
			if (glyphIndex)
				*glyphIndex = cursorGlyph;

			std::size_t glyphCount = textDrawer.GetGlyphCount();
			if (glyphCount > 0 && lineInfo.glyphIndex < cursorGlyph)
			{
				const auto& glyph = textDrawer.GetGlyph(std::min(cursorGlyph, glyphCount - 1));
				return &glyph;
			}
			else
				return nullptr;
		};

		auto& registry = GetRegistry();

		// Move text so that cursor always lies in drawer bounds
		const auto* lastGlyph = GetGlyph(m_cursorPositionEnd, nullptr);
		float glyphPos = (lastGlyph) ? lastGlyph->bounds.x : 0.f;
		float glyphWidth = (lastGlyph) ? lastGlyph->bounds.width : 0.f;

		auto& textNode = registry.get<NodeComponent>(m_textEntity);
		float textPosition = textNode.GetPosition(CoordSys::Local).x - paddingWidth;
		float cursorPosition = glyphPos + textPosition;
		float width = GetWidth();

		if (width <= textDrawer.GetBounds().width)
		{
			if (cursorPosition + glyphWidth > width)
				textNode.Move(width - cursorPosition - glyphWidth, 0.f);
			else if (cursorPosition - glyphWidth < 0.f)
				textNode.Move(-cursorPosition + glyphWidth, 0.f);
		}
		else
			textNode.Move(-textPosition, 0.f); //< Reset text position if we have enough room to show everything

		// Create/destroy cursor entities and sprites
		std::size_t selectionLineCount = m_cursorPositionEnd.y - m_cursorPositionBegin.y + 1;
		std::size_t oldSpriteCount = m_cursors.size();
		if (m_cursors.size() < selectionLineCount)
		{
			m_cursors.resize(selectionLineCount);
			for (std::size_t i = oldSpriteCount; i < m_cursors.size(); ++i)
			{
				m_cursors[i].sprite = std::make_shared<Sprite>(Widgets::Instance()->GetTransparentMaterial());

				m_cursors[i].entity = CreateEntity();
				registry.emplace<GraphicsComponent>(m_cursors[i].entity, HasFocus()).AttachRenderable(m_cursors[i].sprite);
				registry.emplace<NodeComponent>(m_cursors[i].entity).SetParent(textNode);
			}
		}
		else if (m_cursors.size() > selectionLineCount)
		{
			for (std::size_t i = selectionLineCount; i < m_cursors.size(); ++i)
				DestroyEntity(m_cursors[i].entity);

			m_cursors.resize(selectionLineCount);
		}

		// Resize every cursor sprite
		float textHeight = m_textSprite->GetAABB().height;
		for (unsigned int i = m_cursorPositionBegin.y; i <= m_cursorPositionEnd.y; ++i)
		{
			const auto& lineInfo = textDrawer.GetLine(i);

			auto& cursor = m_cursors[i - m_cursorPositionBegin.y];
			if (i == m_cursorPositionBegin.y || i == m_cursorPositionEnd.y)
			{
				// Partial selection (or no selection)
				auto GetGlyphPos = [&](const Vector2ui& glyphPosition)
				{
					std::size_t glyphIndex;
					const auto* glyph = GetGlyph(glyphPosition, &glyphIndex);
					if (glyph)
					{
						float position = glyph->bounds.x;
						if (glyphIndex >= textDrawer.GetGlyphCount())
							position += glyph->bounds.width;

						return position;
					}
					else
						return 0.f;
				};

				float beginX = (i == m_cursorPositionBegin.y) ? GetGlyphPos({ m_cursorPositionBegin.x, i }) : 0.f;
				float endX = (i == m_cursorPositionEnd.y) ? GetGlyphPos({ m_cursorPositionEnd.x, i }) : lineInfo.bounds.width;
				float spriteSize = std::max(endX - beginX, 1.f);

				cursor.sprite->SetColor((m_cursorPositionBegin == m_cursorPositionEnd) ? Color::Black : Color(0, 0, 0, 50));
				cursor.sprite->SetSize(Vector2f(spriteSize, lineInfo.bounds.height));

				registry.get<NodeComponent>(cursor.entity).SetPosition(beginX, textHeight - lineInfo.bounds.y - lineInfo.bounds.height);
			}
			else
			{
				// Full line selection
				cursor.sprite->SetColor(Color(0, 0, 0, 50));
				cursor.sprite->SetSize(Vector2f(lineInfo.bounds.width, lineInfo.bounds.height));

				registry.get<NodeComponent>(cursor.entity).SetPosition(0.f, textHeight - lineInfo.bounds.y - lineInfo.bounds.height);
			}
		}
	}

	void AbstractTextAreaWidget::UpdateTextSprite()
	{
		m_textSprite->Update(GetTextDrawer());
		Vector2f textSize = Vector2f(m_textSprite->GetAABB().GetLengths());
		SetPreferredSize(textSize);

		auto& textNode = GetRegistry().get<NodeComponent>(m_textEntity);
		textNode.SetPosition(paddingWidth, GetHeight() - paddingHeight - textSize.y);
	}
}
