// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDDATAREADER_HPP
#define NAZARA_AUDIO_SOUNDDATAREADER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/Export.hpp>
#include <NazaraUtils/PrivateImpl.hpp>
#include <atomic>
#include <memory>
#include <span>

struct ma_decoder;
using ma_data_source = void;

namespace Nz
{
	class SoundDataSource;

	class NAZARA_AUDIO_API SoundDataReader
	{
		public:
			SoundDataReader(AudioFormat outputFormat, std::span<const AudioChannel> outputChannels, std::uint32_t outputSampleRate, std::shared_ptr<SoundDataSource> source);
			SoundDataReader(const SoundDataReader&) = delete;
			SoundDataReader(SoundDataReader&&) = delete;
			~SoundDataReader();

			ma_data_source* AsDataSource();

			inline void EnableLooping(bool loop);

			inline UInt64 GetReadOffset() const;
			inline const std::shared_ptr<SoundDataSource>& GetSource();

			inline bool IsLooping() const;

			inline void UpdateReadOffset(UInt64 offset);
			inline bool UpdateReadOffset(UInt64 offset, UInt64 expectedPreviousValue);

			SoundDataReader& operator=(const SoundDataReader&) = delete;
			SoundDataReader& operator=(SoundDataReader&&) = delete;

		private:
			struct MiniaudioDataSource; // ma_data_source_base

			PrivateImpl<MiniaudioDataSource, 72, 8> m_miniAudioSource; //< must be the first parameter
			PrivateImpl<ma_decoder, 552, 8> m_miniDecoder;
			std::atomic_bool m_isLooping;
			std::shared_ptr<SoundDataSource> m_source;
			std::atomic_uint64_t m_readOffset;
	};
}

#include <Nazara/Audio/SoundDataReader.inl>

#endif // NAZARA_AUDIO_SOUNDDATAREADER_HPP
