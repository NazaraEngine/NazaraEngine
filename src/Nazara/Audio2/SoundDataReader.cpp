// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/SoundDataReader.hpp>
#include <Nazara/Audio2/MiniaudioUtils.hpp>
#include <Nazara/Audio2/SoundDataSource.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <miniaudio.h>
#include <mutex>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		static constexpr ma_data_source_vtable s_soundDataSourceFuncs =
		{
			.onRead = [](ma_data_source* dataSource, void* framesOut, ma_uint64 frameCount, ma_uint64* frameRead) -> ma_result
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);
				const std::shared_ptr<SoundDataSource>& source = reader->GetSource();

				std::unique_lock<std::mutex> lock;
				if (std::mutex* mutex = source->GetMutex())
					lock = std::unique_lock(*mutex);

				UInt64 cursorPosition = reader->GetReadOffset();
				if (cursorPosition >= source->GetFrameCount())
				{
					if (reader->IsLooping())
						cursorPosition = 0;
					else
						return MA_AT_END;
				}

				auto res = source->Read(cursorPosition, framesOut, frameCount);
				if (!res)
					return MA_ERROR;

				SoundDataSource::ReadData readData = *res;
				*frameRead = readData.readFrame;

				if (reader->IsLooping() && readData.cursorPosition >= source->GetFrameCount())
					readData.cursorPosition = 0;

				reader->UpdateReadOffset(readData.cursorPosition);

				return MA_SUCCESS;
			},
			.onSeek = [](ma_data_source* dataSource, ma_uint64 frameIndex) -> ma_result
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);
				reader->UpdateReadOffset(frameIndex);

				return MA_SUCCESS;
			},
			.onGetDataFormat = [](ma_data_source* dataSource, ma_format* format, ma_uint32* channelCount, ma_uint32* sampleRate, ma_channel* channelMap, std::size_t channelMapCapacity)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);
				const std::shared_ptr<SoundDataSource>& source = reader->GetSource();

				std::span channels = source->GetChannels();

				*format = ToMiniaudio(source->GetFormat());
				*channelCount = SafeCaster(channels.size());
				*sampleRate = source->GetSampleRate();

				std::size_t minChannelCount = std::min<std::size_t>(channelMapCapacity, *channelCount);
				for (std::size_t i = 0; i < minChannelCount; ++i)
					*channelMap++ = ToMiniaudio(channels[i]);

				return MA_SUCCESS;
			},
			.onGetCursor = [](ma_data_source* dataSource, ma_uint64* pCursor)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);

				*pCursor = reader->GetReadOffset();
				return MA_SUCCESS;
			},
			.onGetLength = [](ma_data_source* dataSource, ma_uint64* pLength)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);

				*pLength = reader->GetSource()->GetFrameCount();
				return MA_SUCCESS;
			},
			.onSetLooping = [](ma_data_source* dataSource, ma_bool32 isLooping)
			{
				SoundDataReader* reader = static_cast<SoundDataReader*>(dataSource);
				reader->EnableLooping(isLooping);

				return MA_SUCCESS;
			}
		};

		static ma_decoding_backend_vtable s_customBackendVTable =
		{
			.onInit = [](void* pUserData, ma_read_proc /*onRead*/, ma_seek_proc /*onSeek*/, ma_tell_proc /*onTell*/, void* /*pReadSeekTellUserData*/, const ma_decoding_backend_config* /*pConfig*/, const ma_allocation_callbacks* /*pAllocationCallbacks*/, ma_data_source** ppBackend)
			{
				*ppBackend = pUserData;
				return MA_SUCCESS;
			},
			.onUninit = [](void* /*pUserData*/, ma_data_source* /*pBackend*/, const ma_allocation_callbacks* /*pAllocationCallbacks*/)
			{
			}
		};

		static ma_decoding_backend_vtable* s_customBackendVTables[] =
		{
			&s_customBackendVTable
		};
	}

	struct SoundDataReader::MiniaudioDataSource : ma_data_source_base
	{
	};

	SoundDataReader::SoundDataReader(AudioFormat outputFormat, std::span<const AudioChannel> outputChannels, std::uint32_t outputSampleRate, std::shared_ptr<SoundDataSource> source) :
	m_isLooping(false),
	m_source(std::move(source)),
	m_readOffset(0)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		NazaraAssertMsg(m_source, "invalid source");

		ma_data_source_config config = ma_data_source_config_init();
		config.vtable = &s_soundDataSourceFuncs;

		ma_result result = ma_data_source_init(&config, m_miniAudioSource.Get());
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_data_source_init failed: {}", ma_result_description(result)));

		CallOnExit freeDataSource([&]{ ma_data_source_uninit(m_miniAudioSource.Get()); });

		StackArray<ma_channel> outputChannelMap = NazaraStackArrayNoInit(ma_channel, outputChannels.size());
		for (std::size_t i = 0; i < outputChannels.size(); ++i)
			outputChannelMap[i] = ToMiniaudio(outputChannels[i]);

		ma_decoder_config decoderConfig = ma_decoder_config_init(ToMiniaudio(outputFormat), SafeCaster(outputChannels.size()), outputSampleRate);
		decoderConfig.pChannelMap = outputChannelMap.data();
		decoderConfig.encodingFormat = ma_encoding_format_unknown;
		decoderConfig.ppCustomBackendVTables = s_customBackendVTables;
		decoderConfig.customBackendCount = 1;
		decoderConfig.pCustomBackendUserData = this;

		ma_result decoderResult = ma_decoder_init(nullptr, nullptr, this, &decoderConfig, m_miniDecoder.Get());
		if (decoderResult != MA_SUCCESS)
			throw std::runtime_error(Format("ma_decoder_init failed: {}", ma_result_description(result)));

		freeDataSource.Reset();
	}

	SoundDataReader::~SoundDataReader()
	{
		ma_decoder_uninit(m_miniDecoder.Get());
		ma_data_source_uninit(&m_miniAudioSource);
	}

	ma_data_source* SoundDataReader::AsDataSource()
	{
		return &m_miniDecoder->ds;
	}
}
