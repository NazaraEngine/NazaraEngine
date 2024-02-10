// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_RICHTEXTAREAWIDGET_HPP
#define NAZARA_WIDGETS_RICHTEXTAREAWIDGET_HPP

#include <Nazara/TextRenderer/RichTextDrawer.hpp>
#include <Nazara/Widgets/AbstractTextAreaWidget.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API RichTextAreaWidget : public AbstractTextAreaWidget
	{
		public:
			RichTextAreaWidget(BaseWidget* parent);
			RichTextAreaWidget(const RichTextAreaWidget&) = delete;
			RichTextAreaWidget(RichTextAreaWidget&&) = delete;
			~RichTextAreaWidget() = default;

			void AppendText(std::string_view text);

			void Clear() override;

			void Erase(std::size_t firstGlyph, std::size_t lastGlyph) override;

			inline unsigned int GetCharacterSize() const;
			inline float GetCharacterSpacingOffset() const;
			inline float GetLineSpacingOffset() const;
			inline const Color& GetTextColor() const;
			inline const std::shared_ptr<Font>& GetTextFont() const;
			inline const Color& GetTextOutlineColor() const;
			inline float GetTextOutlineThickness() const;
			inline TextStyleFlags GetTextStyle() const;

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetCharacterSpacingOffset(float offset);
			inline void SetLineSpacingOffset(float offset);
			void SetMaximumTextLength(std::size_t maximumLength) override;
			inline void SetTextColor(const Color& color);
			inline void SetTextFont(std::shared_ptr<Font> font);
			inline void SetTextOutlineColor(const Color& color);
			inline void SetTextOutlineThickness(float thickness);
			inline void SetTextStyle(TextStyleFlags style);

			void Write(std::string_view text, std::size_t glyphPosition) override;

			RichTextAreaWidget& operator=(const RichTextAreaWidget&) = delete;
			RichTextAreaWidget& operator=(RichTextAreaWidget&&) = delete;

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

			RichTextDrawer m_drawer;
	};
}

#include <Nazara/Widgets/RichTextAreaWidget.inl>

#endif // NAZARA_WIDGETS_RICHTEXTAREAWIDGET_HPP
