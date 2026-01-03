// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIOENGINE_HPP
#define NAZARA_AUDIO_AUDIOENGINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioNode.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

struct ma_engine;
struct ma_sound;
struct ma_node_graph;
using ma_sound_group = ma_sound;

namespace Nz
{
	class AudioDevice;
	class Sound;
	class SoundGroup;

	class NAZARA_AUDIO_API AudioEngine : public AudioNode
	{
		public:
			class Endpoint;

			AudioEngine(std::shared_ptr<AudioDevice> audioDevice);
			AudioEngine(const AudioEngine&) = delete;
			AudioEngine(AudioEngine&&) = delete;
			~AudioEngine();

			ma_sound* AllocateInternalSound(std::size_t& soundIndex);
			ma_sound_group* AllocateInternalSoundGroup(std::size_t& soundIndex);

			void FreeInternalSound(std::size_t soundIndex);
			void FreeInternalSoundGroup(std::size_t soundGroupIndex);

			std::uint32_t GetChannelCount() const;
			Endpoint& GetEndpoint();
			const Endpoint& GetEndpoint() const;
			ma_node_graph* GetInternalGraph() const;
			ma_engine* GetInternalHandle() const;
			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;

			void GetListenerCone(UInt32 listenerIndex, RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const;
			Vector3f GetListenerDirection(UInt32 listenerIndex) const;
			Vector3f GetListenerPosition(UInt32 listenerIndex) const;
			Quaternionf GetListenerRotation(UInt32 listenerIndex) const;
			Vector3f GetListenerVelocity(UInt32 listenerIndex) const;
			Vector3f GetListenerWorldUp(UInt32 listenerIndex) const;
			UInt32 GetListenerCount() const;

			std::uint32_t GetSampleRate() const;
			float GetVolume() const;

			bool IsListenerActive(UInt32 listenerIndex) const;

			void SetListenerActive(UInt32 listenerIndex, bool active);
			void SetListenerCone(UInt32 listenerIndex, RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain);
			void SetListenerDirection(UInt32 listenerIndex, const Vector3f& direction);
			void SetListenerPosition(UInt32 listenerIndex, const Vector3f& position);
			void SetListenerRotation(UInt32 listenerIndex, const Quaternionf& rotation);
			void SetListenerVelocity(UInt32 listenerIndex, const Vector3f& velocity);
			void SetListenerWorldUp(UInt32 listenerIndex, const Vector3f& worldUp);
			void SetVolume(float volume);

			void WaitForStateSync();

			AudioEngine& operator=(const AudioEngine&) = delete;
			AudioEngine& operator=(AudioEngine&&) = delete;

		private:
			struct Impl;
			std::unique_ptr<Impl> m_impl;
	};

	class NAZARA_AUDIO_API AudioEngine::Endpoint final : public AudioNode
	{
		friend class AudioEngine;

		public:
			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;

		private:
			ma_node* endpointNode;
	};
}

#include <Nazara/Audio/AudioEngine.inl>

#endif // NAZARA_AUDIO_AUDIOENGINE_HPP
