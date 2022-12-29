// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_AUDIO_MUSIC_HPP
#define NAZARA_AUDIO_MUSIC_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace Nz
{
	class AudioBuffer;

	class NAZARA_AUDIO_API Music final : public Resource, public SoundEmitter
	{
		public:
			Music();
			Music(AudioDevice& device);
			Music(const Music&) = delete;
			Music(Music&&) = delete;
			~Music();

			bool Create(std::shared_ptr<SoundStream> soundStream);
			void Destroy();

			void EnableLooping(bool loop) override;

			Time GetDuration() const override;
			AudioFormat GetFormat() const;
			Time GetPlayingOffset() const;
			UInt64 GetSampleCount() const;
			UInt64 GetSampleOffset() const override;
			UInt32 GetSampleRate() const override;
			SoundStatus GetStatus() const override;

			bool IsLooping() const override;

			bool OpenFromFile(const std::filesystem::path& filePath, const SoundStreamParams& params = SoundStreamParams());
			bool OpenFromMemory(const void* data, std::size_t size, const SoundStreamParams& params = SoundStreamParams());
			bool OpenFromStream(Stream& stream, const SoundStreamParams& params = SoundStreamParams());

			void Pause() override;
			void Play() override;

			void SeekToSampleOffset(UInt64 offset);

			void Stop() override;

			Music& operator=(const Music&) = delete;
			Music& operator=(Music&&) = delete;

		private:
			AudioFormat m_audioFormat;
			std::atomic_bool m_streaming;
			std::atomic<UInt64> m_processedSamples;
			mutable std::recursive_mutex m_sourceLock;
			std::size_t m_bufferCount;
			std::shared_ptr<SoundStream> m_stream;
			std::thread m_thread;
			std::vector<Int16> m_chunkSamples;
			UInt32 m_sampleRate;
			UInt64 m_streamOffset;
			bool m_looping;

			bool FillAndQueueBuffer(std::shared_ptr<AudioBuffer> buffer);
			void MusicThread(std::condition_variable& cv, std::mutex& m, std::exception_ptr& err, bool startPaused);
			void StartThread(bool startPaused);
			void StopThread();
	};
}

#endif // NAZARA_AUDIO_MUSIC_HPP
