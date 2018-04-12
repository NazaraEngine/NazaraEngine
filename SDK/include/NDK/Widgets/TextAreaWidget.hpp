// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_TEXTAREAWIDGET_HPP
#define NDK_WIDGETS_TEXTAREAWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/Widgets/Enums.hpp>

namespace Ndk
{
	class NDK_API TextAreaWidget : public BaseWidget
	{
		public:
			TextAreaWidget(BaseWidget* parent = nullptr);
			TextAreaWidget(const TextAreaWidget&) = delete;
			TextAreaWidget(TextAreaWidget&&) = default;
			~TextAreaWidget() = default;

			void AppendText(const Nz::String& text);

			inline void Clear();

			//virtual TextAreaWidget* Clone() const = 0;

			inline void EnableMultiline(bool enable = true);

			inline unsigned int GetCharacterSize() const;
			inline const Nz::Vector2ui& GetCursorPosition() const;
			inline const Nz::String& GetDisplayText() const;
			inline EchoMode GetEchoMode() const;
			inline std::size_t GetGlyphIndex() const;
			inline std::size_t GetGlyphIndex(const Nz::Vector2ui& cursorPosition) const;
			inline const Nz::String& GetText() const;
			inline const Nz::Color& GetTextColor() const;

			std::size_t GetHoveredGlyph(float x, float y) const;

			inline bool IsMultilineEnabled() const;
			inline bool IsReadOnly() const;

			inline void MoveCursor(int offset);
			inline void MoveCursor(const Nz::Vector2i& offset);

			void ResizeToContent() override;

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetCursorPosition(std::size_t glyphIndex);
			inline void SetCursorPosition(Nz::Vector2ui cursorPosition);
			inline void SetEchoMode(EchoMode echoMode);
			inline void SetReadOnly(bool readOnly = true);
			inline void SetText(const Nz::String& text);
			inline void SetTextColor(const Nz::Color& text);

			void Write(const Nz::String& text);

			TextAreaWidget& operator=(const TextAreaWidget&) = delete;
			TextAreaWidget& operator=(TextAreaWidget&&) = default;

			NazaraSignal(OnTextAreaCursorMove, const TextAreaWidget* /*textArea*/, std::size_t* /*newCursorPosition*/);
			NazaraSignal(OnTextAreaKeyBackspace, const TextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyDown, const TextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyLeft, const TextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyReturn, const TextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyRight, const TextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyUp, const TextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextChanged, const TextAreaWidget* /*textArea*/, const Nz::String& /*text*/);

		private:
			void Layout() override;

			bool IsFocusable() const override;
			void OnFocusLost() override;
			void OnFocusReceived() override;
			bool OnKeyPressed(const Nz::WindowEvent::KeyEvent& key) override;
			void OnKeyReleased(const Nz::WindowEvent::KeyEvent& key) override;
			void OnMouseButtonPress(int /*x*/, int /*y*/, Nz::Mouse::Button button) override;
			void OnTextEntered(char32_t character, bool repeated) override;

			void RefreshCursor();
			void UpdateDisplayText();

			EchoMode m_echoMode;
			EntityHandle m_cursorEntity;
			EntityHandle m_textEntity;
			Nz::SimpleTextDrawer m_drawer;
			Nz::SpriteRef m_cursorSprite;
			Nz::String m_text;
			Nz::TextSpriteRef m_textSprite;
			Nz::Vector2ui m_cursorPosition;
			bool m_multiLineEnabled;
			bool m_readOnly;
	};
}

#include <NDK/Widgets/TextAreaWidget.inl>

#endif // NDK_WIDGETS_TEXTAREAWIDGET_HPP
