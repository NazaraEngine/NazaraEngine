// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup audio
	* \brief Mixes channels in mono
	*
	* \param input Input buffer with multiples channels
	* \param output Output butter for mono
	* \param channelCount Number of channels
	* \param frameCount Number of frames
	*
	* \remark The input buffer may be the same as the output one
	*/
	template<typename T>
	void MixToMono(T* input, T* output, UInt32 channelCount, UInt64 frameCount)
	{
		// To avoid overflow, we use, as an accumulator, a type which is large enough: (u)int 64 bits for integers, double for floatings
		using BiggestInt = typename std::conditional<std::is_unsigned<T>::value, UInt64, Int64>::type;
		using Biggest = typename std::conditional<std::is_integral<T>::value, BiggestInt, double>::type;

		for (UInt64 i = 0; i < frameCount; ++i)
		{
			Biggest acc = Biggest(0);
			for (UInt32 j = 0; j < channelCount; ++j)
				acc += input[i * channelCount + j];

			output[i] = static_cast<T>(acc / channelCount);
		}
	}
}

#include <Nazara/Audio/DebugOff.hpp>
