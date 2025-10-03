// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/Sound.hpp>
#include <Nazara/Audio2/AudioEngine.hpp>
#include <Nazara/Audio2/MiniaudioUtils.hpp>
#include <Nazara/Audio2/SoundDataSource.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <miniaudio.h>
#include <atomic>

namespace Nz
{
	Sound::Sound(Config config)
	{
		NazaraAssertMsg(config.engine, "invalid sound engine");
		ma_engine* engine = config.engine->GetInternalHandle();

		m_sound = config.engine->AllocateInternalSound(m_soundIndex);

		StackArray<AudioChannel> deviceChannelMap = NazaraStackArrayNoInit(AudioChannel, engine->pDevice->playback.channels);
		for (std::size_t i = 0; i < deviceChannelMap.size(); ++i)
			deviceChannelMap[i] = FromMiniaudio(engine->pDevice->playback.channelMap[i]);

		m_sourceReader = std::make_unique<SoundDataReader>(FromMiniaudio(engine->pDevice->playback.format), std::span(deviceChannelMap), engine->pDevice->sampleRate, std::move(config.source));

		ma_sound_config miniaudioConfig = ma_sound_config_init_2(engine);
		miniaudioConfig.channelsOut = MA_SOUND_SOURCE_CHANNEL_COUNT;
		miniaudioConfig.pDataSource = m_sourceReader->AsDataSource();
		if (config.outputNode)
		{
			miniaudioConfig.pInitialAttachment = config.outputNode->GetInternalNode();
			miniaudioConfig.initialAttachmentInputBusIndex = config.outputNodeBus;
		}

		if (!config.attachToNode)
			miniaudioConfig.flags |= MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT;

		if (!config.enablePitching)
			miniaudioConfig.flags |= MA_SOUND_FLAG_NO_PITCH;

		ma_result result = ma_sound_init_ex(engine, &miniaudioConfig, m_sound);
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_sound_init_ex failed: {}", ma_result_description(result)));
	}

	Sound::~Sound()
	{
		if (m_sound)
		{
			ma_sound_uninit(m_sound);
			GetEngine().FreeInternalSound(m_soundIndex);
			GetEngine().WaitUntilCompletion();
		}
	}

	void Sound::EnableLooping(bool loop)
	{
		ma_sound_set_looping(m_sound, loop);
	}

	void Sound::EnableSpatialization(bool spatialization)
	{
		ma_sound_set_spatialization_enabled(m_sound, spatialization);
	}

	SoundAttenuationModel Sound::GetAttenuationModel() const
	{
		return FromMiniaudio(ma_sound_get_attenuation_model(m_sound));
	}

	std::span<const AudioChannel> Sound::GetChannels() const
	{
		return m_sourceReader->GetSource()->GetChannels();
	}

	void Sound::GetCone(RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const
	{
		ma_sound_get_cone(m_sound, &innerAngle.value, &outerAngle.value, &outerGain);
	}

	Vector3f Sound::GetDirection() const
	{
		return FromMiniaudio(ma_sound_get_direction(m_sound));
	}

	Time Sound::GetDuration() const
	{
		return m_sourceReader->GetSource()->GetDuration();
	}

	float Sound::GetDirectionalAttenuationFactor() const
	{
		return ma_sound_get_directional_attenuation_factor(m_sound);
	}

	float Sound::GetDopplerFactor() const
	{
		return ma_sound_get_doppler_factor(m_sound);
	}

	AudioEngine& Sound::GetEngine()
	{
		ma_engine* engine = ma_sound_get_engine(m_sound);
		return *static_cast<AudioEngine*>(engine->pProcessUserData);
	}

	const AudioEngine& Sound::GetEngine() const
	{
		ma_engine* engine = ma_sound_get_engine(m_sound);
		return *static_cast<AudioEngine*>(engine->pProcessUserData);
	}

	AudioFormat Sound::GetFormat() const
	{
		return m_sourceReader->GetSource()->GetFormat();
	}

	UInt64 Sound::GetFrameCount() const
	{
		return m_sourceReader->GetSource()->GetFrameCount();
	}

	ma_node* Sound::GetInternalNode()
	{
		return &m_sound->engineNode;
	}

	const ma_node* Sound::GetInternalNode() const
	{
		return &m_sound->engineNode;
	}

	float Sound::GetMaxDistance() const
	{
		return ma_sound_get_max_distance(m_sound);
	}

	float Sound::GetMaxGain() const
	{
		return ma_sound_get_max_gain(m_sound);
	}

	float Sound::GetMinDistance() const
	{
		return ma_sound_get_min_distance(m_sound);
	}

	float Sound::GetMinGain() const
	{
		return ma_sound_get_min_gain(m_sound);
	}

	float Sound::GetPan() const
	{
		return ma_sound_get_pan(m_sound);
	}

	SoundPanMode Sound::GetPanMode() const
	{
		return FromMiniaudio(ma_sound_get_pan_mode(m_sound));
	}

	float Sound::GetPitch() const
	{
		return ma_sound_get_pitch(m_sound);
	}

	UInt64 Sound::GetPlayingFrame() const
	{
		ma_uint64 playingFrame;
		ma_sound_get_cursor_in_pcm_frames(m_sound, &playingFrame);
		return playingFrame;
	}

	Time Sound::GetPlayingOffset() const
	{
		UInt32 sampleRate = 0;
		ma_sound_get_data_format(m_sound, nullptr, nullptr, &sampleRate, nullptr, 0);
		if NAZARA_UNLIKELY(sampleRate == 0)
			sampleRate = 1; //< avoid division by zero

		UInt64 playingFrame = GetPlayingFrame();
		return Time::Microseconds(playingFrame * 1'000'000ll / sampleRate);
	}

	SoundPositioning Sound::GetPositioning() const
	{
		return FromMiniaudio(ma_sound_get_positioning(m_sound));
	}

	float Sound::GetRolloff() const
	{
		return ma_sound_get_rolloff(m_sound);
	}

	UInt32 Sound::GetListenerIndex() const
	{
		return ma_sound_get_listener_index(m_sound);
	}

	Vector3f Sound::GetPosition() const
	{
		return FromMiniaudio(ma_sound_get_position(m_sound));
	}

	UInt32 Sound::GetSampleRate() const
	{
		return m_sourceReader->GetSource()->GetSampleRate();
	}

	Vector3f Sound::GetVelocity() const
	{
		return FromMiniaudio(ma_sound_get_velocity(m_sound));
	}

	float Sound::GetVolume() const
	{
		return ma_sound_get_volume(m_sound);
	}

	bool Sound::IsLooping() const
	{
		return ma_sound_is_looping(m_sound);
	}

	bool Sound::IsPlaying() const
	{
		return ma_sound_is_playing(m_sound);
	}

	bool Sound::IsSpatializationEnabled() const
	{
		return ma_sound_is_spatialization_enabled(m_sound);
	}

	void Sound::Pause(bool waitUntilCompletion)
	{
		ma_sound_stop(m_sound);

		if (waitUntilCompletion)
			GetEngine().WaitUntilCompletion();
	}

	void Sound::Play(bool waitUntilCompletion)
	{
		ma_sound_start(m_sound);

		if (waitUntilCompletion)
			GetEngine().WaitUntilCompletion();
	}

	void Sound::SeekToFrame(UInt64 frameIndex)
	{
		ma_sound_seek_to_pcm_frame(m_sound, frameIndex);
	}

	void Sound::SeekToTime(Time time)
	{
		// Beware, ma_node are timed around the engine sample rate, not the buffer sample rate
		ma_engine* engine = ma_sound_get_engine(m_sound);
		UInt32 sampleRate = ma_engine_get_sample_rate(engine);
		SeekToFrame(sampleRate * time.AsMicroseconds() / 1'000'000ll);
	}

	void Sound::SetAttenuationModel(SoundAttenuationModel attenuationModel)
	{
		ma_sound_set_attenuation_model(m_sound, ToMiniaudio(attenuationModel));
	}

	void Sound::SetCone(RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain)
	{
		ma_sound_set_cone(m_sound, innerAngle.value, outerAngle.value, outerGain);
	}

	void Sound::SetDirection(const Vector3f& direction)
	{
		ma_sound_set_direction(m_sound, direction.x, direction.y, direction.z);
	}

	void Sound::SetDirectionalAttenuationFactor(float directionalAttenuationFactor)
	{
		ma_sound_set_directional_attenuation_factor(m_sound, directionalAttenuationFactor);
	}

	void Sound::SetDopplerFactor(float dopplerFactor)
	{
		ma_sound_set_doppler_factor(m_sound, dopplerFactor);
	}

	void Sound::SetMaxDistance(float maxDistance)
	{
		ma_sound_set_max_distance(m_sound, maxDistance);
	}

	void Sound::SetMaxGain(float maxGain)
	{
		ma_sound_set_max_gain(m_sound, maxGain);
	}

	void Sound::SetMinDistance(float minDistance)
	{
		ma_sound_set_min_distance(m_sound, minDistance);
	}

	void Sound::SetMinGain(float minGain)
	{
		ma_sound_set_min_gain(m_sound, minGain);
	}

	void Sound::SetPan(float pan)
	{
		ma_sound_set_pan(m_sound, pan);
	}

	void Sound::SetPanMode(SoundPanMode panMode)
	{
		ma_sound_set_pan_mode(m_sound, ToMiniaudio(panMode));
	}

	void Sound::SetPitch(float pitch)
	{
		ma_sound_set_pitch(m_sound, pitch);
	}

	void Sound::SetPositioning(SoundPositioning positioning)
	{
		ma_sound_set_positioning(m_sound, ToMiniaudio(positioning));
	}

	void Sound::SetRolloff(float rollOff)
	{
		ma_sound_set_rolloff(m_sound, rollOff);
	}

	void Sound::SetPosition(const Vector3f& position)
	{
		ma_sound_set_position(m_sound, position.x, position.y, position.z);
	}

	void Sound::SetVelocity(const Vector3f& velocity)
	{
		ma_sound_set_velocity(m_sound, velocity.x, velocity.y, velocity.z);
	}

	void Sound::SetVolume(float volume)
	{
		ma_sound_set_volume(m_sound, volume);
	}

	void Sound::Stop(bool waitUntilCompletion)
	{
		ma_sound_stop(m_sound);
		ma_sound_seek_to_pcm_frame(m_sound, 0);

		if (waitUntilCompletion)
			GetEngine().WaitUntilCompletion();
	}
}
