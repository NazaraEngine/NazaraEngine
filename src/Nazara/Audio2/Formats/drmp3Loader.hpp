// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_FORMATS_DRMP3LOADER_HPP
#define NAZARA_AUDIO2_FORMATS_DRMP3LOADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/SoundBuffer.hpp>
#include <Nazara/Audio2/SoundStream.hpp>

namespace Nz::Loaders
{
	SoundBufferLoader::Entry GetSoundBufferLoader_drmp3();
	SoundStreamLoader::Entry GetSoundStreamLoader_drmp3();
}

#endif // NAZARA_AUDIO2_FORMATS_DRMP3LOADER_HPP
