// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_SNDFILE_HPP
#define NAZARA_LOADERS_SNDFILE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>

namespace Nz::Loaders
{
	SoundBufferLoader::Entry GetSoundBufferLoader_sndfile();
	SoundStreamLoader::Entry GetSoundStreamLoader_sndfile();
}

#endif // NAZARA_LOADERS_SNDFILE_HPP
