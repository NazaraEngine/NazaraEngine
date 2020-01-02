// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/AbstractTextAreaWidget.hpp>
#include <Nazara/Core/Unicode.hpp>
#include <Nazara/Utility/Font.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	namespace
	{
		constexpr float paddingWidth = 5.f;
		constexpr float paddingHeight = 3.f;
	}

	AbstractTextAreaWidget::AbstractTextAreaWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_characterFilter(),
	m_echoMode(EchoMode_Normal),
	m_cursorPositionBegin(0U, 0U),
	m_cursorPositionEnd(0U, 0U),
	m_isLineWrapEnabled(false),
	m_isMouseButtonDown(false),
	m_multiLineEnabled(false),
	m_readOnly(false),
	m_tabEnabled(false)
	{
		m_textSprite = Nz::TextSprite::New();

		m_textEntity = CreateEntity();
		m_textEntity->AddComponent<GraphicsComponent>().Attach(m_textSprite);

		auto& textNode = m_textEntity->AddComponent<NodeComponent>();
		textNode.SetParent(this);
		textNode.SetPosition(paddingWidth, paddingHeight);

		m_cursorEntity = CreateEntity();
		m_cursorEntity->AddComponent<GraphicsComponent>();
		m_cursorEntity->AddComponent<NodeComponent>().SetParent(m_textEntity);
		m_cursorEntity->GetComponent<NodeComponent>();
		m_cursorEntity->Enable(false);

		SetCursor(Nz::SystemCursor_Text);

		EnableBackground(true);
	}

	void AbstractTextAreaWidget::Clear()
	{
		Nz::AbstractTextDrawer& textDrawer = GetTextDrawer();

		m_cursorPositionBegin.MakeZero();
		m_cursorPositionEnd.MakeZero();
		textDrawer.Clear();
		m_textSprite->Update(textDrawer);
		SetPreferredSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));

		RefreshCursor();
	}

	void AbstractTextAreaWidget::EnableLineWrap(bool enable)
	{
		if (m_isLineWrapEnabled != enable)
		{
			m_isLineWrapEnabled = enable;

			Nz::AbstractTextDrawer& textDrawer = GetTextDrawer();

			if (enable)
				textDrawer.SetMaxLineWidth(GetWidth());
			else
				textDrawer.SetMaxLineWidth(std::numeric_limits<float>::infinity());

			UpdateTextSprite();
		}
	}

	Nz::Vector2ui AbstractTextAreaWidget::GetHoveredGlyph(float x, float y) const
	{
		const Nz::AbstractTextDrawer& textDrawer = GetTextDrawer();

		auto& textNode = m_textEntity->GetComponent<Ndk::NodeComponent>();
		Nz::Vector2f textPosition = Nz::Vector2f(textNode.GetPosition(Nz::CoordSys_Local));
		x -= textPosition.x;
		y -= textPosition.y;

		std::size_t glyphCount = textDrawer.GetGlyphCount();
		if (glyphCount > 0)
		{
			std::size_t lineCount = textDrawer.GetLineCount();
			std::size_t line = 0U;
			for (; line < lineCount - 1; ++line)
			{
				Nz::Rectf lineBounds = textDrawer.GetLine(line).bounds;
				if (lineBounds.GetMaximum().y > y)
					break;
			}

			std::size_t upperLimit = (line != lineCount - 1) ? textDrawer.GetLine(line + 1).glyphIndex : glyphCount + 1;

			std::size_t firstLineGlyph = textDrawer.GetLine(line).glyphIndex;
			std::size_t i = firstLineGlyph;
			for (; i < upperLimit - 1; ++i)
			{
				Nz::Rectf bounds = textDrawer.GetGlyph(i).bounds;
				if (x < bounds.x + bounds.width * 0.75f)
					break;
			}

			return Nz::Vector2ui(Nz::Vector2<std::size_t>(i - firstLineGlyph, line));
		}

		return Nz::Vector2ui::Zero();
	}

	void AbstractTextAreaWidget::Layout()
	{
		BaseWidget::Layout();

		if (m_isLineWrapEnabled)
		{
			Nz::AbstractTextDrawer& textDrawer = GetTextDrawer();

			textDrawer.SetMaxLineWidth(GetWidth());
			UpdateTextSprite();
		}

		RefreshCursor();
	}

	bool AbstractTextAreaWidget::IsFocusable() const
	{
		return !m_readOnly;
	}

	void AbstractTextAreaWidget::OnFocusLost()
	{
		m_cursorEntity->Disable();
	}

	void AbstractTextAreaWidget::OnFocusReceived()
	{
		if (!m_readOnly)
			m_cursorEntity->Enable(true);
	}

	bool AbstractTextAreaWidget::OnKeyPressed(const Nz::WindowEvent::KeyEvent& key)
	{
		const Nz::AbstractTextDrawer& textDrawer = GetTextDrawer();

		switch (key.code)
		{
			case Nz::Keyboard::Backspace:
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

			case Nz::Keyboard::Delete:
			{
				if (HasSelection())
					EraseSelection();
				else
					Erase(GetGlyphIndex(m_cursorPositionBegin));

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

				std::size_t lineCount = textDrawer.GetLineCount();
				if (key.control && lineCount > 0)
					SetCursorPosition({ static_cast<unsigned int>(textDrawer.GetLineGlyphCount(lineCount - 1)), static_cast<unsigned int>(lineCount - 1) });
				else
					SetCursorPosition({ static_cast<unsigned int>(textDrawer.GetLineGlyphCount(m_cursorPositionEnd.y)), m_cursorPositionEnd.y });

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
					HandleWordCursorMove(true);
				else
					MoveCursor(-1);

				return true;
			}

			case Nz::Keyboard::Return:
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
				return true;;
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
					HandleWordCursorMove(false);
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

	void AbstractTextAreaWidget::OnKeyReleased(const Nz::WindowEvent::KeyEvent& /*key*/)
	{
	}

	void AbstractTextAreaWidget::OnMouseButtonPress(int x, int y, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
		{
			SetFocus();

			Nz::Vector2ui hoveredGlyph = GetHoveredGlyph(float(x), float(y));

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

	void AbstractTextAreaWidget::OnMouseButtonRelease(int, int, Nz::Mouse::Button button)
	{
		if (button == Nz::Mouse::Left)
			m_isMouseButtonDown = false;
	}

	void AbstractTextAreaWidget::OnMouseEnter()
	{
		if (!Nz::Mouse::IsButtonPressed(Nz::Mouse::Left))
			m_isMouseButtonDown = false;
	}

	void AbstractTextAreaWidget::OnMouseMoved(int x, int y, int deltaX, int deltaY)
	{
		if (m_isMouseButtonDown)
			SetSelection(m_selectionCursor, GetHoveredGlyph(float(x), float(y)));
	}

	void AbstractTextAreaWidget::OnTextEntered(char32_t character, bool /*repeated*/)
	{
		if (m_readOnly)
			return;

		if (Nz::Unicode::GetCategory(character) == Nz::Unicode::Category_Other_Control || (m_characterFilter && !m_characterFilter(character)))
			return;

		if (HasSelection())
			EraseSelection();

		// TODO
		Write(std::string {character});
	}

	void AbstractTextAreaWidget::RefreshCursor()
	{
		if (m_readOnly)
			return;

		const Nz::AbstractTextDrawer& textDrawer = GetTextDrawer();

		auto GetGlyph = [&](const Nz::Vector2ui& glyphPosition, std::size_t* glyphIndex) -> const Nz::AbstractTextDrawer::Glyph*
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

		// Move text so that cursor is always visible
		const auto* lastGlyph = GetGlyph(m_cursorPositionEnd, nullptr);
		float glyphPos = (lastGlyph) ? lastGlyph->bounds.x : 0.f;
		float glyphWidth = (lastGlyph) ? lastGlyph->bounds.width : 0.f;

		auto& node = m_textEntity->GetComponent<Ndk::NodeComponent>();
		float textPosition = node.GetPosition(Nz::CoordSys_Local).x - paddingWidth;
		float cursorPosition = glyphPos + textPosition;
		float width = GetWidth();

		if (width <= textDrawer.GetBounds().width)
		{
			if (cursorPosition + glyphWidth > width)
				node.Move(width - cursorPosition - glyphWidth, 0.f);
			else if (cursorPosition - glyphWidth < 0.f)
				node.Move(-cursorPosition + glyphWidth, 0.f);
		}
		else
			node.Move(-textPosition, 0.f); // Reset text position if we have enough room to show everything

		// Show cursor/selection
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

		GraphicsComponent& gfxComponent = m_cursorEntity->GetComponent<GraphicsComponent>();
		gfxComponent.Clear();

		for (unsigned int i = m_cursorPositionBegin.y; i <= m_cursorPositionEnd.y; ++i)
		{
			const auto& lineInfo = textDrawer.GetLine(i);

			Nz::SpriteRef& cursorSprite = m_cursorSprites[i - m_cursorPositionBegin.y];
			if (i == m_cursorPositionBegin.y || i == m_cursorPositionEnd.y)
			{
				auto GetGlyphPos = [&](const Nz::Vector2ui& glyphPosition)
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

				cursorSprite->SetColor((m_cursorPositionBegin == m_cursorPositionEnd) ? Nz::Color::Black : Nz::Color(0, 0, 0, 50));
				cursorSprite->SetSize(spriteSize, lineInfo.bounds.height);

				gfxComponent.Attach(cursorSprite, Nz::Matrix4f::Translate({ beginX, lineInfo.bounds.y, 0.f }));
			}
			else
			{
				cursorSprite->SetColor(Nz::Color(0, 0, 0, 50));
				cursorSprite->SetSize(lineInfo.bounds.width, lineInfo.bounds.height);

				gfxComponent.Attach(cursorSprite, Nz::Matrix4f::Translate({ 0.f, lineInfo.bounds.y, 0.f }));
			}
		}
	}

	void AbstractTextAreaWidget::UpdateTextSprite()
	{
		m_textSprite->Update(GetTextDrawer());
		SetPreferredSize(Nz::Vector2f(m_textSprite->GetBoundingVolume().obb.localBox.GetLengths()));
	}
}
