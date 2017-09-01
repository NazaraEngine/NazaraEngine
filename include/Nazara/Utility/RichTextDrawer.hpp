// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RICHTEXTDRAWER_HPP
#define NAZARA_RICHTEXTDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/AbstractTextDrawer.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Font.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API RichTextDrawer : public AbstractTextDrawer
	{
		public:
			class BlockRef;

			RichTextDrawer();
			RichTextDrawer(const RichTextDrawer& drawer);
			RichTextDrawer(RichTextDrawer&& drawer);
			virtual ~RichTextDrawer();

			BlockRef Append(const String& str);

			void Clear();

			unsigned int GetBlockCharacterSize(std::size_t index) const;
			const Color& GetBlockColor(std::size_t index) const;
			std::size_t GetBlockCount() const;
			const FontRef& GetBlockFont(std::size_t index) const;
			UInt32 GetBlockStyle(std::size_t index) const;
			const String& GetBlockText(std::size_t index) const;

			unsigned int GetDefaultCharacterSize() const;
			const Color& GetDefaultColor() const;
			const FontRef& GetDefaultFont() const;
			UInt32 GetDefaultStyle() const;

			const Recti& GetBounds() const override;
			Font* GetFont(std::size_t index) const override;
			std::size_t GetFontCount() const override;
			const Glyph& GetGlyph(std::size_t index) const override;
			std::size_t GetGlyphCount() const override;
			const Line& GetLine(std::size_t index) const override;
			std::size_t GetLineCount() const override;

			void MergeBlocks();

			void RemoveBlock(std::size_t index);

			void SetBlockCharacterSize(std::size_t index, unsigned int characterSize);
			void SetBlockColor(std::size_t index, const Color& color);
			void SetBlockFont(std::size_t index, FontRef font);
			void SetBlockStyle(std::size_t index, UInt32 style);
			void SetBlockText(std::size_t index, const String& str);

			void SetDefaultCharacterSize(unsigned int characterSize);
			void SetDefaultColor(const Color& color);
			void SetDefaultFont(FontRef font);
			void SetDefaultStyle(UInt32 style);

			RichTextDrawer& operator=(const RichTextDrawer& drawer);
			RichTextDrawer& operator=(RichTextDrawer&& drawer);

			static RichTextDrawer Draw(const String& str, unsigned int characterSize, UInt32 style = TextStyle_Regular, const Color& color = Color::White);
			static RichTextDrawer Draw(Font* font, const String& str, unsigned int characterSize, UInt32 style = TextStyle_Regular, const Color& color = Color::White);

		private:
			void ClearGlyphs() const;
			void ConnectFontSlots();
			void DisconnectFontSlots();
			void GenerateGlyphs(const String& text) const;
			void OnFontAtlasLayerChanged(const Font* font, AbstractImage* oldLayer, AbstractImage* newLayer);
			void OnFontInvalidated(const Font* font);
			void OnFontRelease(const Font* object);
			void UpdateGlyphs() const;

			NazaraSlot(Font, OnFontAtlasChanged, m_atlasChangedSlot);
			NazaraSlot(Font, OnFontAtlasLayerChanged, m_atlasLayerChangedSlot);
			NazaraSlot(Font, OnFontGlyphCacheCleared, m_glyphCacheClearedSlot);
			NazaraSlot(Font, OnFontRelease, m_fontReleaseSlot);

			struct Block
			{
				Color color;
				String text;
				UInt32 style;
				unsigned int characterSize;
				unsigned int fontIndex;
			};

			Color m_defaultColor;
			FontRef m_defaultFont;
			UInt32 m_defaultStyle;
			unsigned int m_defaultCharacterSize;
			std::unordered_map<FontRef, unsigned int> m_fonts;
			std::vector<Block> m_blocks;
			mutable std::vector<Glyph> m_glyphs;
			mutable Rectf m_workingBounds;
			mutable Rectui m_bounds;
			mutable Vector2ui m_drawPos;
			mutable bool m_glyphUpdated;
	};

	class RichTextDrawer::BlockRef
	{
		public:
			BlockRef(const BlockRef&) = default;
			BlockRef(BlockRef&&) = default;
			~BlockRef() = default;

			inline unsigned int GetCharacterSize() const;
			inline Color GetColor() const;
			inline const FontRef& GetFont() const;
			inline UInt32 GetStyle() const;
			inline const String& GetText() const;

			inline void SetCharacterSize(unsigned int size);
			inline void SetColor(Color color);
			inline void SetFont(FontRef font);
			inline void SetStyle(UInt32 style);
			inline void SetText(const String& text);

			BlockRef& operator=(const BlockRef&) = default;
			BlockRef& operator=(BlockRef&&) = default;

		private:
			inline BlockRef(RichTextDrawer& drawer, std::size_t index);

			std::size_t m_blockIndex;
			RichTextDrawer& m_drawer;
	};
}

#include <Nazara/Utility/RichTextDrawer.inl>

#endif // NAZARA_RICHTEXTDRAWER_HPP
