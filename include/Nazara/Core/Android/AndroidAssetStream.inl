// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline AndroidAssetStream::AndroidAssetStream(std::string filePath, AAsset* asset) :
	Stream(StreamOption::None, OpenMode::Read),
    m_filePath(std::move(filePath)),
    m_asset(asset)
    {
        EnableBuffering(true);
    }
}
