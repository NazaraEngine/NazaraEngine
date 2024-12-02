// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline void AudioDevice::SetDataCallback(DataCallback callback)
	{
		m_dataCallback = std::move(callback);
	}
}
