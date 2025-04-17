// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/SoundGroup.hpp>
#include <Nazara/Audio2/AudioEngine.hpp>
#include <Nazara/Audio2/MiniaudioUtils.hpp>
#include <miniaudio.h>

namespace Nz
{
	SoundGroup::SoundGroup(Config config)
	{
		NazaraAssertMsg(config.engine, "invalid sound engine");
		ma_engine* engine = config.engine->GetInternalHandle();

		m_soundGroup = config.engine->AllocateInternalSoundGroup(m_soundGroupIndex);

		ma_sound_group_config miniaudioConfig = ma_sound_group_config_init_2(engine);
		if (config.node)
			miniaudioConfig.pInitialAttachment = config.node->GetInternalNode();

		ma_result result = ma_sound_group_init_ex(engine, &miniaudioConfig, m_soundGroup);
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_sound_group_init_ex failed: {}", ma_result_description(result)));
	}

	SoundGroup::~SoundGroup()
	{
		if (m_soundGroup)
		{
			GetEngine().FreeInternalSoundGroup(m_soundGroupIndex);
			ma_sound_uninit(m_soundGroup);
		}
	}

	SoundAttenuationModel SoundGroup::GetAttenuationModel() const
	{
		return FromMiniaudio(ma_sound_group_get_attenuation_model(m_soundGroup));
	}

	void SoundGroup::GetCone(RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const
	{
		ma_sound_group_get_cone(m_soundGroup, &innerAngle.value, &outerAngle.value, &outerGain);
	}

	Vector3f SoundGroup::GetDirection() const
	{
		return FromMiniaudio(ma_sound_group_get_direction(m_soundGroup));
	}

	float SoundGroup::GetDirectionalAttenuationFactor() const
	{
		return ma_sound_group_get_directional_attenuation_factor(m_soundGroup);
	}

	float SoundGroup::GetDopplerFactor() const
	{
		return ma_sound_group_get_doppler_factor(m_soundGroup);
	}

	float SoundGroup::GetMaxDistance() const
	{
		return ma_sound_group_get_max_distance(m_soundGroup);
	}

	float SoundGroup::GetMaxGain() const
	{
		return ma_sound_group_get_max_gain(m_soundGroup);
	}

	float SoundGroup::GetMinDistance() const
	{
		return ma_sound_group_get_min_distance(m_soundGroup);
	}

	float SoundGroup::GetMinGain() const
	{
		return ma_sound_group_get_min_gain(m_soundGroup);
	}

	float SoundGroup::GetPan() const
	{
		return ma_sound_group_get_pan(m_soundGroup);
	}

	SoundPanMode SoundGroup::GetPanMode() const
	{
		return FromMiniaudio(ma_sound_group_get_pan_mode(m_soundGroup));
	}

	float SoundGroup::GetPitch() const
	{
		return ma_sound_group_get_pitch(m_soundGroup);
	}

	SoundPositioning SoundGroup::GetPositioning() const
	{
		return FromMiniaudio(ma_sound_group_get_positioning(m_soundGroup));
	}

	float SoundGroup::GetRolloff() const
	{
		return ma_sound_group_get_rolloff(m_soundGroup);
	}

	AudioEngine& SoundGroup::GetEngine()
	{
		ma_engine* engine = ma_sound_group_get_engine(m_soundGroup);
		return *static_cast<AudioEngine*>(engine->pProcessUserData);
	}

	const AudioEngine& SoundGroup::GetEngine() const
	{
		ma_engine* engine = ma_sound_group_get_engine(m_soundGroup);
		return *static_cast<AudioEngine*>(engine->pProcessUserData);
	}

	UInt32 SoundGroup::GetListenerIndex() const
	{
		return ma_sound_group_get_listener_index(m_soundGroup);
	}

	Vector3f SoundGroup::GetPosition() const
	{
		return FromMiniaudio(ma_sound_group_get_position(m_soundGroup));
	}

	Vector3f SoundGroup::GetVelocity() const
	{
		return FromMiniaudio(ma_sound_group_get_velocity(m_soundGroup));
	}

	float SoundGroup::GetVolume() const
	{
		return ma_sound_group_get_volume(m_soundGroup);
	}

	bool SoundGroup::IsPlaying() const
	{
		return ma_sound_group_is_playing(m_soundGroup);
	}

	void SoundGroup::SetAttenuationModel(SoundAttenuationModel attenuationModel)
	{
		ma_sound_group_set_attenuation_model(m_soundGroup, ToMiniaudio(attenuationModel));
	}

	void SoundGroup::SetCone(RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain)
	{
		ma_sound_group_set_cone(m_soundGroup, innerAngle.value, outerAngle.value, outerGain);
	}

	void SoundGroup::SetDirection(const Vector3f& direction)
	{
		ma_sound_group_set_direction(m_soundGroup, direction.x, direction.y, direction.z);
	}

	void SoundGroup::SetDirectionalAttenuationFactor(float directionalAttenuationFactor)
	{
		ma_sound_group_set_directional_attenuation_factor(m_soundGroup, directionalAttenuationFactor);
	}

	void SoundGroup::SetDopplerFactor(float dopplerFactor)
	{
		ma_sound_group_set_doppler_factor(m_soundGroup, dopplerFactor);
	}

	void SoundGroup::SetMaxDistance(float maxDistance)
	{
		ma_sound_group_set_max_distance(m_soundGroup, maxDistance);
	}

	void SoundGroup::SetMaxGain(float maxGain)
	{
		ma_sound_group_set_max_gain(m_soundGroup, maxGain);
	}

	void SoundGroup::SetMinDistance(float minDistance)
	{
		ma_sound_group_set_min_distance(m_soundGroup, minDistance);
	}

	void SoundGroup::SetMinGain(float minGain)
	{
		ma_sound_group_set_min_gain(m_soundGroup, minGain);
	}

	void SoundGroup::SetPan(float pan)
	{
		ma_sound_group_set_pan(m_soundGroup, pan);
	}

	void SoundGroup::SetPanMode(SoundPanMode panMode)
	{
		ma_sound_group_set_pan_mode(m_soundGroup, ToMiniaudio(panMode));
	}

	void SoundGroup::SetPitch(float pitch)
	{
		ma_sound_group_set_pitch(m_soundGroup, pitch);
	}

	void SoundGroup::SetPositioning(SoundPositioning positioning)
	{
		ma_sound_group_set_positioning(m_soundGroup, ToMiniaudio(positioning));
	}

	void SoundGroup::SetRolloff(float rollOff)
	{
		ma_sound_group_set_rolloff(m_soundGroup, rollOff);
	}

	void SoundGroup::SetPosition(const Vector3f& position)
	{
		ma_sound_group_set_velocity(m_soundGroup, position.x, position.y, position.z);
	}

	void SoundGroup::SetVelocity(const Vector3f& velocity)
	{
		ma_sound_group_set_velocity(m_soundGroup, velocity.x, velocity.y, velocity.z);
	}

	void SoundGroup::SetVolume(float volume)
	{
		ma_sound_group_set_volume(m_soundGroup, volume);
	}

	void SoundGroup::Start()
	{
		ma_sound_group_start(m_soundGroup);
	}

	void SoundGroup::Stop()
	{
		ma_sound_group_stop(m_soundGroup);
	}

	ma_node* SoundGroup::GetInternalNode()
	{
		return &m_soundGroup->engineNode;
	}
}
