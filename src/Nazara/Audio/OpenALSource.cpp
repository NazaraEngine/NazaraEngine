// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/OpenALSource.hpp>
#include <Nazara/Audio/OpenALBuffer.hpp>
#include <Nazara/Audio/OpenALDevice.hpp>
#include <Nazara/Audio/OpenALLibrary.hpp>
#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <Nazara/Audio/Debug.hpp>

namespace Nz
{
	OpenALSource::~OpenALSource()
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alDeleteSources(1, &m_sourceId);
	}

	void OpenALSource::EnableLooping(bool loop)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcei(m_sourceId, AL_LOOPING, loop);
	}

	void OpenALSource::EnableSpatialization(bool spatialization)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcei(m_sourceId, AL_SOURCE_RELATIVE, !spatialization);
	}

	float OpenALSource::GetAttenuation() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALfloat attenuation;
		device.alGetSourcefv(m_sourceId, AL_ROLLOFF_FACTOR, &attenuation);

		return attenuation;
	}

	float OpenALSource::GetMinDistance() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALfloat minDistance;
		device.alGetSourcefv(m_sourceId, AL_REFERENCE_DISTANCE, &minDistance);

		return minDistance;
	}

	float OpenALSource::GetPitch() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALfloat pitch;
		device.alGetSourcefv(m_sourceId, AL_PITCH, &pitch);

		return pitch;
	}

	Time OpenALSource::GetPlayingOffset() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

#ifdef AL_SOFT_source_latency
		if (GetDevice().IsExtensionSupported(OpenALExtension::SourceLatency))
		{
			// alGetSourcedvSOFT has extra precision thanks to double
			ALdouble playingOffset;
			device.alGetSourcedvSOFT(m_sourceId, AL_SEC_OFFSET, &playingOffset);

			return Time::Seconds(playingOffset);
		}
		else
#endif
		{
			ALfloat playingOffset;
			device.alGetSourcefv(m_sourceId, AL_SEC_OFFSET, &playingOffset);

			return Time::Seconds(playingOffset);
		}
	}

	Vector3f OpenALSource::GetPosition() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		Vector3f position;
		device.alGetSourcefv(m_sourceId, AL_POSITION, &position.x);

		return position;
	}

	UInt32 OpenALSource::GetSampleOffset() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();


		ALint samples = 0;
		device.alGetSourcei(m_sourceId, AL_SAMPLE_OFFSET, &samples);

		return SafeCast<UInt32>(samples);
	}

	auto OpenALSource::GetSampleOffsetAndLatency() const -> OffsetWithLatency
	{
		OffsetWithLatency offsetWithLatency;
#ifdef AL_SOFT_source_latency
		if (GetDevice().IsExtensionSupported(OpenALExtension::SourceLatency))
		{
			const OpenALDevice& device = GetDevice();
			device.MakeContextCurrent();

			std::array<ALint64SOFT, 2> values;
			device.alGetSourcei64vSOFT(m_sourceId, AL_SAMPLE_OFFSET_LATENCY_SOFT, values.data());

			offsetWithLatency.sampleOffset = ((values[0] & 0xFFFFFFFF00000000) >> 32) * 1'000;
			offsetWithLatency.sourceLatency = Time::Nanoseconds(values[1] / 1'000);

		}
		else
#endif
		{
			offsetWithLatency.sampleOffset = GetSampleOffset() * 1'000;
			offsetWithLatency.sourceLatency = Time::Zero();
		}

		return offsetWithLatency;
	}

	Vector3f OpenALSource::GetVelocity() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		Vector3f velocity;
		device.alGetSourcefv(m_sourceId, AL_VELOCITY, &velocity.x);

		return velocity;
	}

	SoundStatus OpenALSource::GetStatus() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint state;
		device.alGetSourcei(m_sourceId, AL_SOURCE_STATE, &state);

		switch (state)
		{
			case AL_INITIAL:
			case AL_STOPPED:
				return SoundStatus::Stopped;

			case AL_PAUSED:
				return SoundStatus::Paused;

			case AL_PLAYING:
				return SoundStatus::Playing;

			default:
				NazaraInternalError("Source state unrecognized");
		}

		return SoundStatus::Stopped;
	}

	float OpenALSource::GetVolume() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALfloat volume;
		device.alGetSourcefv(m_sourceId, AL_GAIN, &volume);

		return volume;
	}

	bool OpenALSource::IsLooping() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint looping;
		device.alGetSourcei(m_sourceId, AL_LOOPING, &looping);

		return looping == AL_TRUE;
	}

	bool OpenALSource::IsSpatializationEnabled() const
	{
		const OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint relative;
		device.alGetSourcei(m_sourceId, AL_SOURCE_RELATIVE, &relative);

		return relative == AL_FALSE;
	}

	void OpenALSource::QueueBuffer(std::shared_ptr<AudioBuffer> audioBuffer)
	{
		NazaraAssert(audioBuffer, "invalid buffer");
		NazaraAssert(audioBuffer->IsCompatibleWith(*GetAudioDevice()), "incompatible buffer");

		std::shared_ptr<OpenALBuffer> newBuffer = std::static_pointer_cast<OpenALBuffer>(std::move(audioBuffer));

		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();


		ALuint bufferId = newBuffer->GetBufferId();
		device.alSourceQueueBuffers(m_sourceId, 1, &bufferId);

		m_queuedBuffers.emplace_back(std::move(newBuffer));
	}

	void OpenALSource::Pause()
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcePause(m_sourceId);
	}

	void OpenALSource::Play()
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcePlay(m_sourceId);
	}

	void OpenALSource::SetAttenuation(float attenuation)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcef(m_sourceId, AL_ROLLOFF_FACTOR, attenuation);
	}

	void OpenALSource::SetBuffer(std::shared_ptr<AudioBuffer> audioBuffer)
	{
		NazaraAssert(audioBuffer->IsCompatibleWith(*GetAudioDevice()), "incompatible buffer");

		std::shared_ptr<OpenALBuffer> newBuffer = std::static_pointer_cast<OpenALBuffer>(std::move(audioBuffer));

		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		if (newBuffer)
			device.alSourcei(m_sourceId, AL_BUFFER, newBuffer->GetBufferId());
		else
			device.alSourcei(m_sourceId, AL_BUFFER, AL_NONE);

		m_currentBuffer = std::move(newBuffer);
	}

	void OpenALSource::SetMinDistance(float minDistance)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcef(m_sourceId, AL_REFERENCE_DISTANCE, minDistance);
	}

	void OpenALSource::SetPitch(float pitch)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcef(m_sourceId, AL_PITCH, pitch);
	}

	void OpenALSource::SetPlayingOffset(Time offset)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

#ifdef AL_SOFT_source_latency
		if (GetDevice().IsExtensionSupported(OpenALExtension::SourceLatency))
			// alGetSourcedvSOFT has extra precision thanks to double
			device.alSourcedSOFT(m_sourceId, AL_SEC_OFFSET, offset.AsSeconds<ALdouble>());
		else
#endif
			device.alSourcef(m_sourceId, AL_SEC_OFFSET, offset.AsSeconds<ALfloat>());
	}

	void OpenALSource::SetPosition(const Vector3f& position)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSource3f(m_sourceId, AL_POSITION, position.x, position.y, position.z);
	}

	void OpenALSource::SetSampleOffset(UInt32 offset)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcei(m_sourceId, AL_SAMPLE_OFFSET, offset);
	}

	void OpenALSource::SetVelocity(const Vector3f& velocity)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSource3f(m_sourceId, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	}

	void OpenALSource::SetVolume(float volume)
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourcef(m_sourceId, AL_GAIN, volume);
	}

	void OpenALSource::Stop()
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		device.alSourceStop(m_sourceId);
	}

	std::shared_ptr<AudioBuffer> OpenALSource::TryUnqueueProcessedBuffer()
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint processedCount = 0;
		device.alGetSourcei(m_sourceId, AL_BUFFERS_PROCESSED, &processedCount);

		if (processedCount == 0)
			return {};

		ALuint bufferId;
		device.alSourceUnqueueBuffers(m_sourceId, 1, &bufferId);

		auto it = std::find_if(m_queuedBuffers.begin(), m_queuedBuffers.end(), [=](const std::shared_ptr<OpenALBuffer>& alBuffer)
		{
			return alBuffer->GetBufferId() == bufferId;
		});
		assert(it != m_queuedBuffers.end());

		std::shared_ptr<AudioBuffer> buffer = *it;
		m_queuedBuffers.erase(it);

		return buffer;
	}

	void OpenALSource::UnqueueAllBuffers()
	{
		OpenALDevice& device = GetDevice();
		device.MakeContextCurrent();

		ALint queuedBufferCount = 0;
		device.alGetSourcei(m_sourceId, AL_BUFFERS_QUEUED, &queuedBufferCount);

		StackArray<ALuint> buffers = NazaraStackArrayNoInit(ALuint, queuedBufferCount);
		device.alSourceUnqueueBuffers(m_sourceId, queuedBufferCount, buffers.data());

		m_queuedBuffers.clear();
	}

	OpenALDevice& OpenALSource::GetDevice()
	{
		return SafeCast<OpenALDevice&>(*GetAudioDevice());
	}

	const OpenALDevice& OpenALSource::GetDevice() const
	{
		return SafeCast<OpenALDevice&>(*GetAudioDevice());
	}
}
