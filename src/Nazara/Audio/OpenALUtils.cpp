// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALUtils.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	std::string TranslateOpenALError(ALenum code)
	{
		// From https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf
		switch (code)
		{
			case AL_NO_ERROR:
				return "No error";

			case AL_INVALID_NAME:
				return "A bad name (ID) was passed to an OpenAL function";

			case AL_INVALID_ENUM:
				return "An invalid enum value was passed to an OpenAL function";

			case AL_INVALID_VALUE:
				return "An invalid value was passed to an OpenAL function";

			case AL_INVALID_OPERATION:
				return "The requested operation is not valid";

			case AL_OUT_OF_MEMORY:
				return "The requested operation resulted in OpenAL running out of memory";

			default:
				break;
		}

		return Format("Unknown OpenAL error ({0:#x})", code);
	}
}
