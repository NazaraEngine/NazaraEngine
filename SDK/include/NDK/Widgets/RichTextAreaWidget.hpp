// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_WIDGETS_RICHTEXTAREAWIDGET_HPP
#define NDK_WIDGETS_RICHTEXTAREAWIDGET_HPP

#include <NDK/ClientPrerequisites.hpp>
#include <Nazara/Utility/RichTextDrawer.hpp>
#include <NDK/Widgets/AbstractTextAreaWidget.hpp>

namespace Ndk
{
	class NDK_CLIENT_API RichTextAreaWidget : public AbstractTextAreaWidget
	{
		public:
			RichTextAreaWidget(BaseWidget* parent);
			RichTextAreaWidget(const RichTextAreaWidget&) = delete;
			RichTextAreaWidget(RichTextAreaWidget&&) = default;
			~RichTextAreaWidget() = default;

			void AppendText(const Nz::String& text);

			void Clear() override;

			void Erase(std::size_t firstGlyph, std::size_t lastGlyph) override;

			inline unsigned int GetCharacterSize() const;
			inline float GetCharacterSpacingOffset() const;
			inline float GetLineSpacingOffset() const;
			inline const Nz::Color& GetTextColor() const;
			inline Nz::Font* GetTextFont() const;
			inline const Nz::Color& GetTextOutlineColor() const;
			inline float GetTextOutlineThickness() const;
			inline Nz::TextStyleFlags GetTextStyle() const;

			inline void SetCharacterSize(unsigned int characterSize);
			inline void SetCharacterSpacingOffset(float offset);
			inline void SetLineSpacingOffset(float offset);
			inline void SetTextColor(const Nz::Color& color);
			inline void SetTextFont(Nz::FontRef font);
			inline void SetTextOutlineColor(const Nz::Color& color);
			inline void SetTextOutlineThickness(float thickness);
			inline void SetTextStyle(Nz::TextStyleFlags style);

			void Write(const Nz::String& text, std::size_t glyphPosition) override;

			RichTextAreaWidget& operator=(const RichTextAreaWidget&) = delete;
			RichTextAreaWidget& operator=(RichTextAreaWidget&&) = default;

		private:
			Nz::AbstractTextDrawer& GetTextDrawer() override;
			const Nz::AbstractTextDrawer& GetTextDrawer() const override;

			void HandleIndentation(bool add) override;
			void HandleSelectionIndentation(bool add) override;
			void HandleWordCursorMove(bool left) override;

			void UpdateDisplayText() override;

			Nz::RichTextDrawer m_drawer;
	};
}

#include <NDK/Widgets/RichTextAreaWidget.inl>

#endif // NDK_WIDGETS_TEXTAREAWIDGET_HPP
