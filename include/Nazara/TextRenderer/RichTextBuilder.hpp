// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_TEXTRENDERER_RICHTEXTBUILDER_HPP
#define NAZARA_TEXTRENDERER_RICHTEXTBUILDER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/TextRenderer/Enums.hpp>
#include <Nazara/TextRenderer/Font.hpp>
#include <memory>
#include <string>

namespace Nz
{
	class Font;

	template<typename T>
	class RichTextBuilder
	{
		public:
			struct CharacterSizeWrapper;
			struct CharacterSpacingOffsetWrapper;
			struct LineSpacingOffsetWrapper;
			struct OutlineColorWrapper;
			struct OutlineThicknessWrapper;

			RichTextBuilder(T& richText);
			RichTextBuilder(T* richText);
			RichTextBuilder(const RichTextBuilder&) = delete;
			RichTextBuilder(RichTextBuilder&&) = delete;
			~RichTextBuilder() = default;

			inline RichTextBuilder& AppendText(std::string_view text);

			inline RichTextBuilder& SetCharacterSize(unsigned int characterSize);
			inline RichTextBuilder& SetCharacterSpacingOffset(float offset);
			inline RichTextBuilder& SetLineSpacingOffset(float offset);
			inline RichTextBuilder& SetTextColor(const Color& color);
			inline RichTextBuilder& SetTextFont(const std::shared_ptr<Font>& font);
			inline RichTextBuilder& SetTextOutlineColor(const Color& color);
			inline RichTextBuilder& SetTextOutlineThickness(float thickness);
			inline RichTextBuilder& SetTextStyle(TextStyleFlags style);

			RichTextBuilder& operator=(const RichTextBuilder&) = delete;
			RichTextBuilder& operator=(RichTextBuilder&&) = delete;

			inline RichTextBuilder& operator<<(const Color& textColor);
			inline RichTextBuilder& operator<<(const std::shared_ptr<Font>& font);
			inline RichTextBuilder& operator<<(std::string_view str);
			inline RichTextBuilder& operator<<(CharacterSizeWrapper characterSize);
			inline RichTextBuilder& operator<<(CharacterSpacingOffsetWrapper characterSpacingOffset);
			inline RichTextBuilder& operator<<(LineSpacingOffsetWrapper lineSpacing);
			inline RichTextBuilder& operator<<(OutlineColorWrapper outlineColor);
			inline RichTextBuilder& operator<<(OutlineThicknessWrapper outlineThickness);
			inline RichTextBuilder& operator<<(TextStyleFlags textStyle);

			static inline CharacterSizeWrapper CharacterSize(unsigned int characterSize);
			static inline CharacterSpacingOffsetWrapper CharacterSpacingOffset(unsigned int spacingOffset);
			static inline LineSpacingOffsetWrapper LineSpacingOffset(float spacingOffset);
			static inline OutlineColorWrapper OutlineColor(const Color& color);
			static inline OutlineThicknessWrapper OutlineThickness(float thickness);

			struct CharacterSizeWrapper { unsigned int characterSize; };
			struct CharacterSpacingOffsetWrapper { float spacingOffset; };
			struct LineSpacingOffsetWrapper { float spacingOffset; };
			struct OutlineColorWrapper { const Color& color; };
			struct OutlineThicknessWrapper { float thickness; };

		private:
			T& m_richText;
	};

}

#include <Nazara/TextRenderer/RichTextBuilder.inl>

#endif // NAZARA_TEXTRENDERER_RICHTEXTBUILDER_HPP
