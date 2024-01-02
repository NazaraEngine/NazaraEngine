// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_FORMATS_MINIMP3LOADER_HPP
#define NAZARA_AUDIO_FORMATS_MINIMP3LOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>

namespace Nz::Loaders
{
	SoundBufferLoader::Entry GetSoundBufferLoader_minimp3();
	SoundStreamLoader::Entry GetSoundStreamLoader_minimp3();
}

#endif // NAZARA_AUDIO_FORMATS_MINIMP3LOADER_HPP
