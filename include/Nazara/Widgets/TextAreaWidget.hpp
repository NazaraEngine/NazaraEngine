// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_WIDGETS_TEXTAREAWIDGET_HPP
#define NAZARA_WIDGETS_TEXTAREAWIDGET_HPP

#include <Nazara/TextRenderer/SimpleTextDrawer.hpp>
#include <Nazara/Widgets/AbstractTextAreaWidget.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API TextAreaWidget : public AbstractTextAreaWidget
	{
		public:
			TextAreaWidget(BaseWidget* parent);
			TextAreaWidget(const TextAreaWidget&) = delete;
			TextAreaWidget(TextAreaWidget&&) = delete;
			~TextAreaWidget() = default;

			void AppendText(std::string_view text);

			void Clear() override;

			using AbstractTextAreaWidget::Erase;
			void Erase(std::size_t firstGlyph, std::size_t lastGlyph) override;

			inline unsigned int GetCharacterSize() const;
			inline const std::string& GetDisplayText() const;
			inline float GetCharacterSpacingOffset() const;
			inline float GetLineSpacingOffset() const;
			inline const std::string& GetText() const;
			inline const Color& GetTextColor() const;
			inline const std::shared_ptr<Font>& GetTextFont() const;
			inline const Color& GetTextOulineColor() const;
			inline float GetTextOulineThickness() const;
			inline TextStyleFlags GetTextStyle() const;

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetCharacterSpacingOffset(float offset);
			inline void SetLineSpacingOffset(float offset);
			void SetMaximumTextLength(std::size_t maximumLength) override;
			inline void SetText(std::string text);
			inline void SetTextColor(const Color& text);
			inline void SetTextFont(std::shared_ptr<Font> font);
			inline void SetTextOutlineColor(const Color& color);
			inline void SetTextOutlineThickness(float thickness);
			inline void SetTextStyle(TextStyleFlags style);

			using AbstractTextAreaWidget::Write;
			void Write(std::string_view text, std::size_t glyphPosition) override;

			TextAreaWidget& operator=(const TextAreaWidget&) = delete;
			TextAreaWidget& operator=(TextAreaWidget&&) = delete;

			NazaraSignal(OnTextChanged, const AbstractTextAreaWidget* /*textArea*/, const std::string& /*text*/);

		private:
			void CopySelectionToClipboard(const Vector2ui& selectionBegin, const Vector2ui& selectionEnd) override;

			AbstractTextDrawer& GetTextDrawer() override;
			const AbstractTextDrawer& GetTextDrawer() const override;

			void HandleIndentation(bool add) override;
			void HandleSelectionIndentation(bool add) override;
			void HandleWordCursorMove(bool left) override;
			void HandleWordSelection(const Vector2ui& position) override;

			void PasteFromClipboard(const Vector2ui& targetPosition) override;

			void UpdateDisplayText() override;
			void UpdateMinimumSize();

			SimpleTextDrawer m_drawer;
			std::string m_text;
	};
}

#include <Nazara/Widgets/TextAreaWidget.inl>

#endif // NAZARA_WIDGETS_TEXTAREAWIDGET_HPP
