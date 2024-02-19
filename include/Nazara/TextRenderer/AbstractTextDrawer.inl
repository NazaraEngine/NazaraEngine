// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Text renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline std::size_t AbstractTextDrawer::GetLineGlyphCount(std::size_t index) const
	{
		std::size_t lineCount = GetLineCount();
		const auto& lineInfo = GetLine(index);
		if (index == lineCount - 1)
			return GetGlyphCount() - lineInfo.glyphIndex;

		const auto& nextLineInfo = GetLine(index + 1);

		return nextLineInfo.glyphIndex - lineInfo.glyphIndex;
	}
}

