// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_OPENALUTILS_HPP
#define NAZARA_AUDIO_OPENALUTILS_HPP

#if defined(NAZARA_AUDIO_OPENAL) || defined(NAZARA_AUDIO_BUILD)

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Audio/OpenAL.hpp>
#include <string>

namespace Nz
{
	NAZARA_AUDIO_API std::string TranslateOpenALError(ALenum code);
}

#endif // NAZARA_AUDIO_OPENAL

#endif // NAZARA_AUDIO_OPENALUTILS_HPP
