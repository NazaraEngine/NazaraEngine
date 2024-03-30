// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Android/AndroidAssetStream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline AndroidAssetStream::AndroidAssetStream(std::string filePath, AAsset* asset) :
	Stream(StreamOption::None, OpenMode::ReadOnly),
    m_filePath(std::move(filePath)),
    m_asset(asset)
    {
    }
}

#include <Nazara/Core/DebugOff.hpp>
