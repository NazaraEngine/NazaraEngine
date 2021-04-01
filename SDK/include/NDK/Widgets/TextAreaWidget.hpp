// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_TEXTAREAWIDGET_HPP
#define NDK_WIDGETS_TEXTAREAWIDGET_HPP

#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/ClientPrerequisites.hpp>
#include <NDK/Widgets/AbstractTextAreaWidget.hpp>

namespace Ndk
{
	class NDK_CLIENT_API TextAreaWidget : public AbstractTextAreaWidget
	{
		public:
			TextAreaWidget(BaseWidget* parent);
			TextAreaWidget(const TextAreaWidget&) = delete;
			TextAreaWidget(TextAreaWidget&&) = default;
			~TextAreaWidget() = default;

			void AppendText(const Nz::String& text);

			void Clear() override;

			using AbstractTextAreaWidget::Erase;
			void Erase(std::size_t firstGlyph, std::size_t lastGlyph) override;

			inline unsigned int GetCharacterSize() const;
			inline const Nz::String& GetDisplayText() const;
			inline float GetCharacterSpacingOffset() const;
			inline float GetLineSpacingOffset() const;
			inline const Nz::String& GetText() const;
			inline const Nz::Color& GetTextColor() const;
			inline Nz::Font* GetTextFont() const;
			inline const Nz::Color& GetTextOulineColor() const;
			inline float GetTextOulineThickness() const;
			inline Nz::TextStyleFlags GetTextStyle() const;

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetCharacterSpacingOffset(float offset);
			inline void SetLineSpacingOffset(float offset);
			inline void SetText(const Nz::String& text);
			inline void SetTextColor(const Nz::Color& text);
			inline void SetTextFont(Nz::FontRef font);
			inline void SetTextOutlineColor(const Nz::Color& color);
			inline void SetTextOutlineThickness(float thickness);
			inline void SetTextStyle(Nz::TextStyleFlags style);

			using AbstractTextAreaWidget::Write;
			void Write(const Nz::String& text, std::size_t glyphPosition) override;

			TextAreaWidget& operator=(const TextAreaWidget&) = delete;
			TextAreaWidget& operator=(TextAreaWidget&&) = default;

			NazaraSignal(OnTextChanged, const AbstractTextAreaWidget* /*textArea*/, const Nz::String& /*text*/);

		private:
			Nz::AbstractTextDrawer& GetTextDrawer() override;
			const Nz::AbstractTextDrawer& GetTextDrawer() const override;

			void HandleIndentation(bool add) override;
			void HandleSelectionIndentation(bool add) override;
			void HandleWordCursorMove(bool left) override;

			void UpdateDisplayText() override;
			void UpdateMinimumSize();

			Nz::SimpleTextDrawer m_drawer;
			Nz::String m_text;
	};
}

#include <NDK/Widgets/TextAreaWidget.inl>

#endif // NDK_WIDGETS_TEXTAREAWIDGET_HPP
