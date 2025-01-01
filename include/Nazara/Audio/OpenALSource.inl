// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline OpenALSource::OpenALSource(std::shared_ptr<AudioDevice> device, ALuint sourceId) :
	AudioSource(std::move(device)),
	m_sourceId(sourceId)
	{
	}
}
