// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/AudioEngine.hpp>
#include <Nazara/Audio/AudioDevice.hpp>
#include <Nazara/Audio/MiniaudioUtils.hpp>
#include <Nazara/Core/Format.hpp>
#include <NazaraUtils/MemoryPool.hpp>
#include <miniaudio.h>
#include <mutex>
#include <stdexcept>

namespace Nz
{
	struct AudioEngine::Impl
	{
		Impl() :
		soundPool(128)
		{
		}

		std::shared_ptr<AudioDevice> device;
		std::mutex readMutex;
		MemoryPool<ma_sound> soundPool;
		Endpoint endpoint;
		ma_engine engine;
	};

	AudioEngine::AudioEngine(std::shared_ptr<AudioDevice> audioDevice)
	{
		NazaraAssertMsg(audioDevice, "invalid audio device");

		m_impl = std::make_unique<Impl>();
		m_impl->device = std::move(audioDevice);

		ma_engine_config engineConfig = ma_engine_config_init();
		engineConfig.pDevice = m_impl->device->GetInternalDevice();
		engineConfig.pProcessUserData = this;

		ma_result result = ma_engine_init(&engineConfig, &m_impl->engine);
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_engine_init failed: {}", ma_result_description(result)));

		m_impl->endpoint.endpointNode = ma_engine_get_endpoint(&m_impl->engine);

		// Normally ma_engine sets the device data callback to ma_engine_data_callback_internal but we don't have access to that function
		// and we want to keep userdata pointing to AudioDevice, so do the job ourselves
		m_impl->device->SetDataCallback([this](const AudioDevice& /*device*/, const void* /*inputData*/, void* outputData, UInt32 frameCount)
		{
			std::unique_lock lock(m_impl->readMutex);
			ma_engine_read_pcm_frames(&m_impl->engine, outputData, frameCount, nullptr);
		});
	}

	AudioEngine::~AudioEngine()
	{
		if (std::size_t allocatedSounds = m_impl->soundPool.GetAllocatedEntryCount(); allocatedSounds != 0)
			NazaraWarning("{} sound(s) are still allocated at engine destruction!", allocatedSounds);

		m_impl->device->SetDataCallback(nullptr);
		ma_engine_uninit(&m_impl->engine);
	}

	ma_sound* AudioEngine::AllocateInternalSound(std::size_t& soundIndex)
	{
		return m_impl->soundPool.Allocate(soundIndex);
	}

	ma_sound_group* AudioEngine::AllocateInternalSoundGroup(std::size_t& soundGroupIndex)
	{
		// We use the same pool for sound and sound groups
		static_assert(std::is_same_v<ma_sound, ma_sound_group>);
		return m_impl->soundPool.Allocate(soundGroupIndex);
	}

	void AudioEngine::FreeInternalSound(std::size_t soundIndex)
	{
		m_impl->soundPool.Free(soundIndex);
	}

	void AudioEngine::FreeInternalSoundGroup(std::size_t soundGroupIndex)
	{
		// We use the same pool for sound and sound groups
		static_assert(std::is_same_v<ma_sound, ma_sound_group>);
		m_impl->soundPool.Free(soundGroupIndex);
	}

	std::uint32_t AudioEngine::GetChannelCount() const
	{
		return ma_engine_get_channels(&m_impl->engine);
	}

	auto AudioEngine::GetEndpoint() -> Endpoint&
	{
		return m_impl->endpoint;
	}

	auto AudioEngine::GetEndpoint() const -> const Endpoint&
	{
		return m_impl->endpoint;
	}

	std::uint32_t AudioEngine::GetSampleRate() const
	{
		return ma_engine_get_sample_rate(&m_impl->engine);
	}

	ma_node_graph* AudioEngine::GetInternalGraph() const
	{
		return ma_engine_get_node_graph(GetInternalHandle());
	}

	ma_engine* AudioEngine::GetInternalHandle() const
	{
		// some ma_engine API are not const-correct
		return const_cast<ma_engine*>(static_cast<const ma_engine*>(&m_impl->engine));
	}

	ma_node* AudioEngine::GetInternalNode()
	{
		return &m_impl->engine.nodeGraph.base;
	}

	const ma_node* AudioEngine::GetInternalNode() const
	{
		return &m_impl->engine.nodeGraph.base;
	}

	void AudioEngine::GetListenerCone(UInt32 listenerIndex, RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const
	{
		ma_engine_listener_get_cone(&m_impl->engine, listenerIndex, &innerAngle.value, &outerAngle.value, &outerGain);
	}

	Vector3f AudioEngine::GetListenerDirection(UInt32 listenerIndex) const
	{
		return FromMiniaudio(ma_engine_listener_get_direction(&m_impl->engine, listenerIndex));
	}

	Vector3f AudioEngine::GetListenerPosition(UInt32 listenerIndex) const
	{
		return FromMiniaudio(ma_engine_listener_get_position(&m_impl->engine, listenerIndex));
	}

	Quaternionf AudioEngine::GetListenerRotation(UInt32 listenerIndex) const
	{
		Vector3f direction = GetListenerDirection(listenerIndex);
		Vector3f worldUp = GetListenerWorldUp(listenerIndex);

		return Quaternionf::LookAt(direction, worldUp);
	}

	Vector3f AudioEngine::GetListenerVelocity(UInt32 listenerIndex) const
	{
		return FromMiniaudio(ma_engine_listener_get_velocity(&m_impl->engine, listenerIndex));
	}

	Vector3f AudioEngine::GetListenerWorldUp(UInt32 listenerIndex) const
	{
		return FromMiniaudio(ma_engine_listener_get_world_up(&m_impl->engine, listenerIndex));
	}

	UInt32 AudioEngine::GetListenerCount() const
	{
		return ma_engine_get_listener_count(&m_impl->engine);
	}

	float AudioEngine::GetVolume() const
	{
		return ma_engine_get_volume(GetInternalHandle()); 
	}

	bool AudioEngine::IsListenerActive(UInt32 listenerIndex) const
	{
		return ma_engine_listener_is_enabled(&m_impl->engine, listenerIndex) == MA_TRUE;
	}

	void AudioEngine::SetListenerActive(UInt32 listenerIndex, bool active)
	{
		ma_engine_listener_set_enabled(&m_impl->engine, listenerIndex, (active) ? MA_TRUE : MA_FALSE);
	}

	void AudioEngine::SetListenerCone(UInt32 listenerIndex, RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain)
	{
		ma_engine_listener_set_cone(&m_impl->engine, listenerIndex, innerAngle.value, outerAngle.value, outerGain);
	}

	void AudioEngine::SetListenerDirection(UInt32 listenerIndex, const Vector3f& direction)
	{
		ma_engine_listener_set_direction(&m_impl->engine, listenerIndex, direction.x, direction.y, direction.z);
	}

	void AudioEngine::SetListenerPosition(UInt32 listenerIndex, const Vector3f& position)
	{
		ma_engine_listener_set_direction(&m_impl->engine, listenerIndex, position.x, position.y, position.z);
	}

	void AudioEngine::SetListenerRotation(UInt32 listenerIndex, const Quaternionf& rotation)
	{
		SetListenerDirection(listenerIndex, rotation * Nz::Vector3f::Forward());
		SetListenerWorldUp(listenerIndex, rotation * Nz::Vector3f::Up());
	}

	void AudioEngine::SetListenerVelocity(UInt32 listenerIndex, const Vector3f& velocity)
	{
		ma_engine_listener_set_velocity(&m_impl->engine, listenerIndex, velocity.x, velocity.y, velocity.z);
	}

	void AudioEngine::SetListenerWorldUp(UInt32 listenerIndex, const Vector3f& worldUp)
	{
		ma_engine_listener_set_world_up(&m_impl->engine, listenerIndex, worldUp.x, worldUp.y, worldUp.z);
	}

	void AudioEngine::SetVolume(float volume)
	{
		ma_engine_set_volume(&m_impl->engine, volume);
	}

	void AudioEngine::WaitForStateSync()
	{
		// Lock and unlock mutex to ensure we're not in a audio thread cycle (which means states set before calling this will be read by the audio thread)
		std::unique_lock lock(m_impl->readMutex);
	}

	ma_node* AudioEngine::Endpoint::GetInternalNode()
	{
		return endpointNode;
	}

	const ma_node* AudioEngine::Endpoint::GetInternalNode() const
	{
		return endpointNode;
	}
}
