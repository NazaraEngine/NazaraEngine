// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline std::size_t AbstractTextDrawer::GetLineGlyphCount(std::size_t index) const
	{
		const AbstractTextDrawer::Line* lines = GetLines();

		std::size_t lineCount = GetLineCount();
		const auto& lineInfo = lines[index];
		if (index == lineCount - 1)
			return GetGlyphCount() - lineInfo.glyphIndex;

		const auto& nextLineInfo = lines[index + 1];

		return nextLineInfo.glyphIndex - lineInfo.glyphIndex;
	}
}
