// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Android/VideoModeImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Platform/VideoMode.hpp>
#include <algorithm>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	VideoMode VideoModeImpl::GetDesktopMode()
	{
		return VideoMode(800, 600, static_cast<UInt8>(32));
	}

	void VideoModeImpl::GetFullscreenModes(std::vector<VideoMode>& modes)
	{
		modes.push_back(GetDesktopMode());
	}
}
