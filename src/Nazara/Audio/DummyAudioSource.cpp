// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/DummyAudioSource.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/DummyAudioBuffer.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <algorithm>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	void DummyAudioSource::EnableLooping(bool loop)
	{
		m_isLooping = loop;
	}

	void DummyAudioSource::EnableSpatialization(bool spatialization)
	{
		m_isSpatialized = spatialization;
	}

	float DummyAudioSource::GetAttenuation() const
	{
		return m_attenuation;
	}

	float DummyAudioSource::GetMinDistance() const
	{
		return m_minDistance;
	}

	float DummyAudioSource::GetPitch() const
	{
		return m_pitch;
	}

	Vector3f DummyAudioSource::GetPosition() const
	{
		return m_position;
	}

	UInt32 DummyAudioSource::GetSampleOffset() const
	{
		if (m_status == SoundStatus::Stopped)
			return 0; //< Always return 0 when stopped, to mimic OpenAL behavior

		UInt64 bufferTime = UpdateTime();

		UInt64 sampleOffset = 0;
		// All processed buffers count in sample offset
		for (const auto& processedBuffer : m_processedBuffers)
			sampleOffset += processedBuffer->GetSampleCount() / GetChannelCount(processedBuffer->GetAudioFormat());

		if (!m_queuedBuffers.empty())
		{
			auto& frontBuffer = m_queuedBuffers.front();
			UInt64 bufferOffset = bufferTime * frontBuffer->GetSampleRate() / 1000;
			UInt64 bufferDuration = frontBuffer->GetSampleCount() / GetChannelCount(frontBuffer->GetAudioFormat());

			sampleOffset += std::min(bufferOffset, bufferDuration);
		}

		return SafeCast<UInt32>(sampleOffset);
	}

	auto DummyAudioSource::GetSampleOffsetAndLatency() const -> OffsetWithLatency
	{
		OffsetWithLatency info;
		info.sampleOffset = GetSampleOffset() * 1000;
		info.sourceLatency = 0;

		return info;
	}

	Vector3f DummyAudioSource::GetVelocity() const
	{
		return m_velocity;
	}

	SoundStatus DummyAudioSource::GetStatus() const
	{
		UpdateTime();

		return m_status;
	}

	float DummyAudioSource::GetVolume() const
	{
		return m_volume;
	}

	bool DummyAudioSource::IsLooping() const
	{
		return m_isLooping;
	}

	bool DummyAudioSource::IsSpatializationEnabled() const
	{
		return m_isSpatialized;
	}

	void DummyAudioSource::QueueBuffer(std::shared_ptr<AudioBuffer> audioBuffer)
	{
		NazaraAssert(audioBuffer, "invalid buffer");
		NazaraAssert(audioBuffer->IsCompatibleWith(*GetAudioDevice()), "incompatible buffer");

		m_queuedBuffers.emplace_back(std::static_pointer_cast<DummyAudioBuffer>(audioBuffer));
	}

	void DummyAudioSource::Pause()
	{
		m_playClock.Pause();
		m_status = SoundStatus::Paused;
	}

	void DummyAudioSource::Play()
	{
		if (m_status == SoundStatus::Paused)
			m_playClock.Unpause();
		else
		{
			// playing or stopped, restart
			RequeueBuffers();

			// special case, we are stopped but SetSampleOffset has been called
			if (m_status == SoundStatus::Stopped && m_playClock.GetMilliseconds() != 0)
				m_playClock.Unpause();
			else
				m_playClock.Restart(); //< already playing or stopped, restart from beginning
		}

		m_status = SoundStatus::Playing;
	}

	void DummyAudioSource::SetAttenuation(float attenuation)
	{
		m_attenuation = attenuation;
	}

	void DummyAudioSource::SetBuffer(std::shared_ptr<AudioBuffer> audioBuffer)
	{
		NazaraAssert(audioBuffer->IsCompatibleWith(*GetAudioDevice()), "incompatible buffer");

		m_queuedBuffers.clear();
		m_queuedBuffers.emplace_back(std::static_pointer_cast<DummyAudioBuffer>(audioBuffer));
		m_processedBuffers.clear();
	}

	void DummyAudioSource::SetMinDistance(float minDistance)
	{
		m_minDistance = minDistance;
	}

	void DummyAudioSource::SetPitch(float pitch)
	{
		m_pitch = pitch;
	}

	void DummyAudioSource::SetPosition(const Vector3f& position)
	{
		m_position = position;
	}

	void DummyAudioSource::SetSampleOffset(UInt32 offset)
	{
		RequeueBuffers();

		if (m_queuedBuffers.empty())
			return;

		std::size_t processedBufferIndex = 0;
		for (; processedBufferIndex < m_queuedBuffers.size(); ++processedBufferIndex)
		{
			UInt32 bufferFrameCount = SafeCast<UInt32>(m_queuedBuffers[processedBufferIndex]->GetSampleCount() / GetChannelCount(m_queuedBuffers[processedBufferIndex]->GetAudioFormat()));
			if (offset < bufferFrameCount)
				break;

			offset -= bufferFrameCount;
			m_processedBuffers.emplace_back(std::move(m_queuedBuffers[processedBufferIndex]));
		}
		m_queuedBuffers.erase(m_queuedBuffers.begin(), m_queuedBuffers.begin() + processedBufferIndex);

		if (!m_queuedBuffers.empty())
		{
			UInt64 timeOffset = 1'000'000ULL * offset / m_queuedBuffers.front()->GetSampleRate();
			m_playClock.Restart(timeOffset, m_playClock.IsPaused());
		}
		else
			Stop();
	}

	void DummyAudioSource::SetVelocity(const Vector3f& velocity)
	{
		m_velocity = velocity;
	}

	void DummyAudioSource::SetVolume(float volume)
	{
		m_volume = volume;
	}

	void DummyAudioSource::Stop()
	{
		m_playClock.Restart(0, true);
		m_status = SoundStatus::Stopped;
	}

	std::shared_ptr<AudioBuffer> DummyAudioSource::TryUnqueueProcessedBuffer()
	{
		UpdateTime();

		if (m_processedBuffers.empty())
			return {};

		auto processedBuffer = std::move(m_processedBuffers.front());
		m_processedBuffers.erase(m_processedBuffers.begin());

		return processedBuffer;
	}

	void DummyAudioSource::UnqueueAllBuffers()
	{
		m_processedBuffers.clear();
		m_queuedBuffers.clear();
		Stop();
	}

	void DummyAudioSource::RequeueBuffers()
	{
		// Put back all processed buffers in the queued buffer queue (for simplicity)
		if (!m_processedBuffers.empty())
		{
			m_queuedBuffers.resize(m_processedBuffers.size() + m_queuedBuffers.size());

			// Move currently queued buffers to the end of the queue
			if (m_queuedBuffers.size() > m_processedBuffers.size())
				std::move(m_queuedBuffers.begin(), m_queuedBuffers.begin() + m_processedBuffers.size(), m_queuedBuffers.begin() + m_processedBuffers.size());

			std::move(m_processedBuffers.begin(), m_processedBuffers.end(), m_queuedBuffers.begin());
			m_processedBuffers.clear();
		}
	}

	UInt64 DummyAudioSource::UpdateTime() const
	{
		UInt64 currentTime = m_playClock.GetMilliseconds();

		while (!m_queuedBuffers.empty() && currentTime >= m_queuedBuffers.front()->GetDuration())
		{
			auto processedBuffer = std::move(m_queuedBuffers.front());
			m_queuedBuffers.erase(m_queuedBuffers.begin());

			currentTime -= processedBuffer->GetDuration();

			m_processedBuffers.emplace_back(std::move(processedBuffer));
		}

		if (m_queuedBuffers.empty())
		{
			// If looping, replay processed buffers
			if (m_isLooping)
			{
				while (!m_processedBuffers.empty())
				{
					auto queuedBuffer = std::move(m_processedBuffers.front());
					m_processedBuffers.erase(m_processedBuffers.begin());

					m_queuedBuffers.emplace_back(std::move(queuedBuffer));
					if (m_queuedBuffers.back()->GetDuration() > currentTime)
						break;

					currentTime -= m_queuedBuffers.back()->GetDuration();
				}
			}
			else
				m_status = SoundStatus::Stopped;
		}

		m_playClock.Restart(currentTime * 1000, m_playClock.IsPaused()); //< Adjust time
		return currentTime;
	}
}
