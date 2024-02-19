// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	template<typename T>
	RichTextBuilder<T>::RichTextBuilder(T& richText) :
	m_richText(richText)
	{
	}

	template<typename T>
	RichTextBuilder<T>::RichTextBuilder(T* richText) :
	m_richText(*richText)
	{
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::AppendText(std::string_view text)
	{
		m_richText.AppendText(text);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetCharacterSize(unsigned int characterSize)
	{
		m_richText.SetCharacterSize(characterSize);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetCharacterSpacingOffset(float offset)
	{
		m_richText.SetCharacterSpacingOffset(offset);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetLineSpacingOffset(float offset)
	{
		m_richText.SetLineSpacingOffset(offset);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetTextColor(const Color& color)
	{
		m_richText.SetTextColor(color);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetTextFont(const std::shared_ptr<Font>& font)
	{
		m_richText.SetTextFont(font);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetTextOutlineColor(const Color& color)
	{
		m_richText.SetTextOutlineColor(color);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetTextOutlineThickness(float thickness)
	{
		m_richText.SetTextOutlineThickness(thickness);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::SetTextStyle(TextStyleFlags style)
	{
		m_richText.SetTextStyle(style);
		return *this;
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(const Color& textColor)
	{
		return SetTextColor(textColor);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(const std::shared_ptr<Font>& font)
	{
		return SetTextFont(font);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(std::string_view str)
	{
		return AppendText(str);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(CharacterSizeWrapper characterSize)
	{
		return SetCharacterSize(characterSize.characterSize);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(CharacterSpacingOffsetWrapper characterSpacingOffset)
	{
		return SetCharacterSpacingOffset(characterSpacingOffset.spacingOffset);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(LineSpacingOffsetWrapper lineSpacing)
	{
		return SetLineSpacingOffset(lineSpacing.spacingOffset);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(OutlineColorWrapper outlineColor)
	{
		return SetTextOutlineColor(outlineColor.color);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(OutlineThicknessWrapper outlineThickness)
	{
		return SetTextOutlineThickness(outlineThickness.thickness);
	}

	template<typename T>
	RichTextBuilder<T>& RichTextBuilder<T>::operator<<(TextStyleFlags textStyle)
	{
		return SetTextStyle(textStyle);
	}

	template<typename T>
	auto RichTextBuilder<T>::CharacterSize(unsigned int characterSize) -> CharacterSizeWrapper
	{
		return { characterSize };
	}

	template<typename T>
	auto RichTextBuilder<T>::CharacterSpacingOffset(unsigned int spacingOffset) -> CharacterSpacingOffsetWrapper
	{
		return { spacingOffset };
	}

	template<typename T>
	auto RichTextBuilder<T>::LineSpacingOffset(float spacingOffset) -> LineSpacingOffsetWrapper
	{
		return { spacingOffset };
	}

	template<typename T>
	auto RichTextBuilder<T>::OutlineColor(const Color& color) -> OutlineColorWrapper
	{
		return { color };
	}

	template<typename T>
	auto RichTextBuilder<T>::OutlineThickness(float thickness) -> OutlineThicknessWrapper
	{
		return { thickness };
	}
}

