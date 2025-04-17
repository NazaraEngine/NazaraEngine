// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/SoundDataReader.hpp>
#include <Nazara/Audio2/MiniaudioUtils.hpp>
#include <Nazara/Audio2/SoundDataSource.hpp>
#include <miniaudio.h>
#include <mutex>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		static ma_data_source_vtable s_soundDataSourceFuncs =
		{
			// onRead
			[](ma_data_source* dataSource, void* framesOut, ma_uint64 frameCount, ma_uint64* frameRead) -> ma_result
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);

				std::unique_lock<std::mutex> lock;
				if (std::mutex* mutex = reader->GetSource()->GetMutex())
					lock = std::unique_lock(*mutex);

				auto res = reader->GetSource()->Read(reader->GetReadOffset(), framesOut, frameCount);
				if (!res)
					return MA_ERROR;

				SoundDataSource::ReadData readData = *res;
				*frameRead = readData.readFrame;

				reader->UpdateReadOffset(readData.cursorPosition);

				return MA_SUCCESS;
			},
			// onSeek
			[](ma_data_source* dataSource, ma_uint64 frameIndex) -> ma_result
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);
				reader->UpdateReadOffset(frameIndex);

				return MA_SUCCESS;
			},
			// onGetDataFormat
			[](ma_data_source* dataSource, ma_format* format, ma_uint32* channelCount, ma_uint32* sampleRate, ma_channel* channelMap, std::size_t channelMapCapacity)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);

				std::span channels = reader->GetSource()->GetChannels();

				*format = ToMiniaudio(reader->GetSource()->GetFormat());
				*channelCount = channels.size();
				*sampleRate = reader->GetSource()->GetSampleRate();

				std::size_t minChannelCount = std::min<std::size_t>(channelMapCapacity, *channelCount);
				for (std::size_t i = 0; i < minChannelCount; ++i)
					*channelMap++ = ToMiniaudio(channels[i]);

				return MA_SUCCESS;
			},
			// onGetCursor
			[](ma_data_source* dataSource, ma_uint64* pCursor)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);

				*pCursor = reader->GetReadOffset();
				return MA_SUCCESS;
			},
			// onGetLength
			[](ma_data_source* dataSource, ma_uint64* pLength)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);

				*pLength = reader->GetSource()->GetFrameCount();
				return MA_SUCCESS;
			},
			// onSetLooping
			nullptr
		};
	}

	struct SoundDataReader::MiniaudioDataSource : ma_data_source_base
	{
	};

	SoundDataReader::SoundDataReader(std::shared_ptr<SoundDataSource> source) :
	m_source(std::move(source)),
	m_readOffset(0)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		ma_data_source_config config = ma_data_source_config_init();
		config.vtable = &s_soundDataSourceFuncs;

		ma_result result = ma_data_source_init(&config, &m_miniAudioSource);
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_data_source_init failed: {}", ma_result_description(result)));
	}

	SoundDataReader::~SoundDataReader()
	{
		ma_data_source_uninit(&m_miniAudioSource);
	}

	ma_data_source* SoundDataReader::AsDataSource()
	{
		return m_miniAudioSource.Get();
	}
}
