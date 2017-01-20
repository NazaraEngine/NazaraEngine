// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_WIDGETS_TEXTAREAWIDGET_HPP
#define NDK_WIDGETS_TEXTAREAWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/BaseWidget.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <Nazara/Graphics/TextSprite.hpp>

namespace Ndk
{
	class World;

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

			inline std::size_t GetCursorPosition() const;
			inline std::size_t GetLineCount() const;
			inline const Nz::String& GetText() const;
			inline const Nz::Color& GetTextColor() const;

			std::size_t GetHoveredGlyph(float x, float y) const;

			inline bool IsMultilineEnabled() const;
			inline bool IsReadOnly() const;

			inline void MoveCursor(int offset);

			void ResizeToContent() override;

			inline void SetCursorPosition(std::size_t cursorPosition);
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

		private:
			void Layout() override;

			void OnKeyPressed(const Nz::WindowEvent::KeyEvent& key) override;
			void OnKeyReleased(const Nz::WindowEvent::KeyEvent& key) override;
			void OnMouseEnter() override;
			void OnMouseButtonPress(int /*x*/, int /*y*/, Nz::Mouse::Button button) override;
			void OnMouseMoved(int x, int y, int deltaX, int deltaY) override;
			void OnMouseExit() override;
			void OnTextEntered(char32_t character, bool repeated) override;

			void RefreshCursor();

			EntityHandle m_cursorEntity;
			EntityHandle m_textEntity;
			Nz::SimpleTextDrawer m_drawer;
			Nz::SpriteRef m_cursorSprite;
			Nz::TextSpriteRef m_textSprite;
			std::size_t m_cursorPosition;
			bool m_multiLineEnabled;
			bool m_readOnly;
	};
}

#include <NDK/Widgets/TextAreaWidget.inl>

#endif // NDK_WIDGETS_TEXTAREAWIDGET_HPP
