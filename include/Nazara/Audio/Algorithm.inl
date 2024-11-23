// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	/*!
	* \ingroup audio
	* \brief Get the number of channels occupied by an audio format
	* \returns The number of channels occupied by an audio format (mono returns 1, stereo returns 2, etc.)
	*
	* \param format A valid audio format
	*
	* \remark format cannot be AudioFormat::Unknown
	*
	* \see GuessAudioFormat
	*/
	UInt32 GetChannelCount(AudioFormat format)
	{
		NazaraAssertMsg(format != AudioFormat::Unknown, "invalid audio format");

		switch (format)
		{
			case AudioFormat::Unknown: //< Just to make the compiler stop complaining
				break;

			case AudioFormat::I16_Mono:
				return 1;

			case AudioFormat::I16_Stereo:
				return 2;

			case AudioFormat::I16_Quad:
				return 4;

			case AudioFormat::I16_5_1:
				return 6;

			case AudioFormat::I16_6_1:
				return 7;

			case AudioFormat::I16_7_1:
				return 8;
		}

		return 0;
	}

	/*!
	* \ingroup audio
	* \brief Gets the common audio format associated with a specific channel count
	* \returns AudioFormat associated with channel count or empty optional if none match
	*
	* \param channelCount Channel count
	*
	* \see GetChannelCount
	*/
	inline std::optional<AudioFormat> GuessAudioFormat(UInt32 channelCount)
	{
		switch (channelCount)
		{
			case 1:
				return AudioFormat::I16_Mono;

			case 2:
				return AudioFormat::I16_Stereo;

			case 4:
				return AudioFormat::I16_Quad;

			case 6:
				return AudioFormat::I16_5_1;

			case 7:
				return AudioFormat::I16_6_1;

			case 8:
				return AudioFormat::I16_7_1;

			default:
				return std::nullopt;
		}
	}

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

