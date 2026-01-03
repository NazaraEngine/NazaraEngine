// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUND_HPP
#define NAZARA_AUDIO_SOUND_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioNode.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Audio/SoundDataReader.hpp>
#include <Nazara/Audio/SoundInterface.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/MovableValue.hpp>

struct ma_sound;

namespace Nz
{
	class NAZARA_AUDIO_API Sound final : public AudioNode, public SoundInterface
	{
		public:
			struct Config;

			Sound(Config config);
			Sound(const Sound&) = delete;
			Sound(Sound&&) noexcept = default;
			~Sound();

			void EnableLooping(bool loop);
			void EnableSpatialization(bool spatialization);

			SoundAttenuationModel GetAttenuationModel() const override;
			std::span<const AudioChannel> GetChannels() const;
			void GetCone(RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const override;
			Vector3f GetDirection() const override;
			Time GetDuration() const;
			float GetDirectionalAttenuationFactor() const override;
			float GetDopplerFactor() const override;
			AudioEngine& GetEngine() override;
			const AudioEngine& GetEngine() const override;
			AudioFormat GetFormat() const;
			UInt64 GetFrameCount() const;
			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;
			float GetMaxDistance() const override;
			float GetMaxGain() const override;
			float GetMinDistance() const override;
			float GetMinGain() const override;
			float GetPan() const override;
			SoundPanMode GetPanMode() const override;
			float GetPitch() const override;
			UInt64 GetPlayingFrame() const;
			Time GetPlayingOffset() const;
			SoundPositioning GetPositioning() const override;
			float GetRolloff() const override;
			UInt32 GetListenerIndex() const override;
			Vector3f GetPosition() const override;
			UInt32 GetSampleRate() const;
			Vector3f GetVelocity() const override;
			float GetVolume() const override;

			bool IsLooping() const;
			bool IsPlaying() const override;
			bool IsSpatializationEnabled() const;

			void Pause(bool waitUntilCompletion = false);
			void Play(bool waitUntilCompletion = false);

			void SeekToFrame(UInt64 time);
			void SeekToTime(Time time);

			void SetAttenuationModel(SoundAttenuationModel attenuationModel) override;
			void SetCone(RadianAnglef innerAngle, RadianAnglef outerAngle, float outerGain) override;
			void SetDirection(const Vector3f& direction) override;
			void SetDirectionalAttenuationFactor(float directionalAttenuationFactor) override;
			void SetDopplerFactor(float dopplerFactor) override;
			void SetMaxDistance(float maxDistance) override;
			void SetMaxGain(float maxGain) override;
			void SetMinDistance(float minDistance) override;
			void SetMinGain(float minGain) override;
			void SetPan(float pan) override;
			void SetPanMode(SoundPanMode panMode) override;
			void SetPitch(float pitch) override;
			void SetPositioning(SoundPositioning positioning) override;
			void SetRolloff(float rollOff) override;
			void SetPosition(const Vector3f& position) override;
			void SetVelocity(const Vector3f& velocity) override;
			void SetVolume(float volume) override;

			void Stop(bool waitUntilCompletion = false);

			Sound& operator=(const Sound&) = delete;
			Sound& operator=(Sound&&) noexcept = default;

			struct Config
			{
				AudioEngine* engine;

				std::shared_ptr<SoundDataSource> source;
				AudioNode* outputNode = nullptr;
				UInt32 outputNodeBus = 0;
				bool attachToNode = true;
				bool enablePitching = false;
			};

		private:
			std::unique_ptr<SoundDataReader> m_sourceReader;
			MovableLiteral<std::size_t, MaxValue()> m_soundIndex;
			MovablePtr<ma_sound> m_sound;
	};
}

#include <Nazara/Audio/Sound.inl>

#endif // NAZARA_AUDIO_SOUND_HPP
