// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_ABSTRACTTEXTAREAWIDGET_HPP
#define NAZARA_WIDGETS_ABSTRACTTEXTAREAWIDGET_HPP

#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/TextRenderer/AbstractTextDrawer.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/Enums.hpp>
#include <Nazara/Widgets/WidgetStyleFactory.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>
#include <functional>
#include <vector>

namespace Nz
{
	class TextAreaWidgetStyle;

	class NAZARA_WIDGETS_API AbstractTextAreaWidget : public BaseWidget
	{
		public:
			using CharacterFilter = std::function<bool(char32_t)>;
			using StyleFactory = WidgetStyleFactory<AbstractTextAreaWidget, TextAreaWidgetStyle>;

			AbstractTextAreaWidget(BaseWidget* parent, const StyleFactory& styleFactory = nullptr);
			AbstractTextAreaWidget(const AbstractTextAreaWidget&) = delete;
			AbstractTextAreaWidget(AbstractTextAreaWidget&&) = delete;
			~AbstractTextAreaWidget() = default;

			inline void Disable();
			void Enable(bool enable = true);

			virtual void Clear();

			void EnableBackground(bool enable) override;
			void EnableLineWrap(bool enable = true);
			inline void EnableMultiline(bool enable = true);
			inline void EnableTabWriting(bool enable = true);

			inline void Erase(std::size_t glyphPosition);
			virtual void Erase(std::size_t firstGlyph, std::size_t lastGlyph) = 0;
			inline void EraseSelection();

			inline const CharacterFilter& GetCharacterFilter() const;
			inline const Vector2ui& GetCursorPosition() const;
			inline Vector2ui GetCursorPosition(std::size_t glyphIndex) const;
			inline EchoMode GetEchoMode() const;
			inline std::size_t GetGlyphIndex() const;
			inline std::size_t GetGlyphIndex(const Vector2ui& cursorPosition) const;
			inline std::size_t GetMaximumTextLength() const;
			inline std::pair<Vector2ui, Vector2ui> GetSelection() const;
			inline Vector2f GetTextPadding() const;

			Vector2ui GetHoveredGlyph(float x, float y) const;

			inline bool HasSelection() const;

			inline bool IsEnabled() const;
			inline bool IsLineWrapEnabled() const;
			inline bool IsMultilineEnabled() const;
			inline bool IsReadOnly() const;
			inline bool IsTabWritingEnabled() const;

			inline void MoveCursor(int offset);
			inline void MoveCursor(const Vector2i& offset);

			inline Vector2ui NormalizeCursorPosition(Vector2ui cursorPosition) const;

			void SetBackgroundColor(const Color& color) override;
			inline void SetCharacterFilter(CharacterFilter filter);
			inline void SetCursorPosition(std::size_t glyphIndex);
			inline void SetCursorPosition(Vector2ui cursorPosition);
			inline void SetEchoMode(EchoMode echoMode);
			virtual void SetMaximumTextLength(std::size_t maximumLength) = 0;
			inline void SetReadOnly(bool readOnly = true);
			inline void SetSelection(Vector2ui fromPosition, Vector2ui toPosition);
			inline void SetTextPadding(Vector2f textPadding);

			inline void Write(std::string_view text);
			inline void Write(std::string_view text, const Vector2ui& glyphPosition);
			virtual void Write(std::string_view text, std::size_t glyphPosition) = 0;

			AbstractTextAreaWidget& operator=(const AbstractTextAreaWidget&) = delete;
			AbstractTextAreaWidget& operator=(AbstractTextAreaWidget&&) = delete;

			NazaraSignal(OnTextAreaCursorMove, const AbstractTextAreaWidget* /*textArea*/, Vector2ui* /*newCursorPosition*/);
			NazaraSignal(OnTextAreaKeyBackspace, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyCopy, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyCut, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyDown, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyEnd, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyHome, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyLeft, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyPaste, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyReturn, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyRight, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaKeyUp, const AbstractTextAreaWidget* /*textArea*/, bool* /*ignoreDefaultAction*/);
			NazaraSignal(OnTextAreaSelection, const AbstractTextAreaWidget* /*textArea*/, Vector2ui* /*start*/, Vector2ui* /*end*/);

		protected:
			virtual void CopySelectionToClipboard(const Vector2ui& selectionBegin, const Vector2ui& selectionEnd) = 0;

			virtual AbstractTextDrawer& GetTextDrawer() = 0;
			virtual const AbstractTextDrawer& GetTextDrawer() const = 0;

			virtual void HandleIndentation(bool add) = 0;
			virtual void HandleSelectionIndentation(bool add) = 0;
			virtual void HandleWordCursorMove(bool left) = 0;
			virtual void HandleWordSelection(const Vector2ui& position) = 0;

			bool IsFocusable() const override;
			void Layout() override;

			void OnFocusLost() override;
			void OnFocusReceived() override;
			bool OnKeyPressed(const WindowEvent::KeyEvent& key) override;
			bool OnMouseButtonDoublePress(int x, int y, Mouse::Button button) override;
			bool OnMouseButtonPress(int x, int y, Mouse::Button button) override;
			bool OnMouseButtonRelease(int x, int y, Mouse::Button button) override;
			bool OnMouseButtonTriplePress(int x, int y, Mouse::Button button) override;
			void OnMouseEnter() override;
			bool OnMouseMoved(int x, int y, int deltaX, int deltaY) override;
			void OnRenderLayerUpdated(int baseRenderLayer) override;
			bool OnTextEntered(char32_t character, bool repeated) override;

			virtual void PasteFromClipboard(const Vector2ui& targetPosition) = 0;

			void RefreshCursorSize();

			inline void SetCursorPositionInternal(std::size_t glyphIndex);
			inline void SetCursorPositionInternal(Vector2ui cursorPosition);

			virtual void UpdateDisplayText() = 0;
			void UpdateTextOffset(float offset);
			void UpdateTextSprite();

			std::size_t m_maximumTextLength;
			std::unique_ptr<TextAreaWidgetStyle> m_style;
			std::vector<Rectf> m_cursorRects;
			CharacterFilter m_characterFilter;
			EchoMode m_echoMode;
			Vector2f m_textPadding;
			Vector2ui m_cursorPositionBegin;
			Vector2ui m_cursorPositionEnd;
			Vector2ui m_selectionCursor;
			bool m_isEnabled;
			bool m_isLineWrapEnabled;
			bool m_isMouseButtonDown;
			bool m_multiLineEnabled;
			bool m_readOnly;
			bool m_tabEnabled; // writes (Shift+)Tab character if set to true
			float m_textOffset;
	};
}

#include <Nazara/Widgets/AbstractTextAreaWidget.inl>

#endif // NAZARA_WIDGETS_ABSTRACTTEXTAREAWIDGET_HPP
