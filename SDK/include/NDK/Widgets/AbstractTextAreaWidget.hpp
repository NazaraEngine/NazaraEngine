// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_ABSTRACTTEXTAREAWIDGET_HPP
#define NDK_WIDGETS_ABSTRACTTEXTAREAWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <NDK/BaseWidget.hpp>
#include <NDK/ClientPrerequisites.hpp>
#include <NDK/Widgets/Enums.hpp>
#include <functional>
#include <vector>

namespace Ndk
{
	class NDK_CLIENT_API AbstractTextAreaWidget : public BaseWidget
	{
		public:
			using CharacterFilter = std::function<bool(char32_t)>;

			AbstractTextAreaWidget(BaseWidget* parent);
			AbstractTextAreaWidget(const AbstractTextAreaWidget&) = delete;
			AbstractTextAreaWidget(AbstractTextAreaWidget&&) = default;
			~AbstractTextAreaWidget() = default;

			virtual void Clear();

			//virtual TextAreaWidget* Clone() const = 0;

			void EnableLineWrap(bool enable = true);
			inline void EnableMultiline(bool enable = true);
			inline void EnableTabWriting(bool enable = true);

			inline void Erase(std::size_t glyphPosition);
			virtual void Erase(std::size_t firstGlyph, std::size_t lastGlyph) = 0;
			inline void EraseSelection();

			inline const CharacterFilter& GetCharacterFilter() const;
			inline const Nz::Vector2ui& GetCursorPosition() const;
			inline Nz::Vector2ui GetCursorPosition(std::size_t glyphIndex) const;
			inline EchoMode GetEchoMode() const;
			inline std::size_t GetGlyphIndex() const;
			inline std::size_t GetGlyphIndex(const Nz::Vector2ui& cursorPosition) const;
			inline const Nz::String& GetText() const;

			Nz::Vector2ui GetHoveredGlyph(float x, float y) const;

			inline bool HasSelection() const;

			inline bool IsLineWrapEnabled() const;
			inline bool IsMultilineEnabled() const;
			inline bool IsReadOnly() const;
			inline bool IsTabWritingEnabled() const;

			inline void MoveCursor(int offset);
			inline void MoveCursor(const Nz::Vector2i& offset);

			inline Nz::Vector2ui NormalizeCursorPosition(Nz::Vector2ui cursorPosition) const;

			inline void SetCharacterFilter(CharacterFilter filter);
			inline void SetCursorPosition(std::size_t glyphIndex);
			inline void SetCursorPosition(Nz::Vector2ui cursorPosition);
			inline void SetEchoMode(EchoMode echoMode);
			inline void SetReadOnly(bool readOnly = true);
			inline void SetSelection(Nz::Vector2ui fromPosition, Nz::Vector2ui toPosition);

			inline void Write(const Nz::String& text);
			inline void Write(const Nz::String& text, const Nz::Vector2ui& glyphPosition);
			virtual void Write(const Nz::String& text, std::size_t glyphPosition) = 0;

			AbstractTextAreaWidget& operator=(const AbstractTextAreaWidget&) = delete;
			AbstractTextAreaWidget& operator=(AbstractTextAreaWidget&&) = default;

			NazaraSignal(OnTextAreaCursorMove, const AbstractTextAreaWidget* /*textArea*/, Nz::Vector2ui* /*newCursorPosition*/);
			NazaraSignal(OnTextAreaKeyBackspace, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyDown, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyEnd, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyHome, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyLeft, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyReturn, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyRight, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyUp, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaSelection, const AbstractTextAreaWidget* /*textArea*/, Nz::Vector2ui* /*start*/, Nz::Vector2ui* /*end*/);

		protected:
			virtual Nz::AbstractTextDrawer& GetTextDrawer() = 0;
			virtual const Nz::AbstractTextDrawer& GetTextDrawer() const = 0;

			void Layout() override;

			virtual void HandleIndentation(bool add) = 0;
			virtual void HandleSelectionIndentation(bool add) = 0;
			virtual void HandleWordCursorMove(bool left) = 0;

			bool IsFocusable() const override;
			void OnFocusLost() override;
			void OnFocusReceived() override;
			bool OnKeyPressed(const Nz::WindowEvent::KeyEvent& key) override;
			void OnKeyReleased(const Nz::WindowEvent::KeyEvent& key) override;
			void OnMouseButtonPress(int /*x*/, int /*y*/, Nz::Mouse::Button button) override;
			void OnMouseButtonRelease(int /*x*/, int /*y*/, Nz::Mouse::Button button) override;
			void OnMouseEnter() override;
			void OnMouseMoved(int x, int y, int deltaX, int deltaY) override;
			void OnTextEntered(char32_t character, bool repeated) override;

			inline void SetCursorPositionInternal(std::size_t glyphIndex);
			inline void SetCursorPositionInternal(Nz::Vector2ui cursorPosition);

			void RefreshCursor();
			virtual void UpdateDisplayText() = 0;
			void UpdateTextSprite();

			CharacterFilter m_characterFilter;
			EchoMode m_echoMode;
			EntityHandle m_cursorEntity;
			EntityHandle m_textEntity;
			Nz::TextSpriteRef m_textSprite;
			Nz::Vector2ui m_cursorPositionBegin;
			Nz::Vector2ui m_cursorPositionEnd;
			Nz::Vector2ui m_selectionCursor;
			std::vector<Nz::SpriteRef> m_cursorSprites;
			bool m_isLineWrapEnabled;
			bool m_isMouseButtonDown;
			bool m_multiLineEnabled;
			bool m_readOnly;
			bool m_tabEnabled; // writes (Shift+)Tab character if set to true
	};
}

#include <NDK/Widgets/AbstractTextAreaWidget.inl>

#endif // NDK_WIDGETS_ABSTRACTTEXTAREAWIDGET_HPP
