// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_SOUNDGROUP_HPP
#define NAZARA_AUDIO_SOUNDGROUP_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioNode.hpp>
#include <Nazara/Audio/Export.hpp>
#include <Nazara/Audio/SoundInterface.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/MovableValue.hpp>

struct ma_sound;
using ma_sound_group = ma_sound;

namespace Nz
{
	class NAZARA_AUDIO_API SoundGroup final : public AudioNode, public SoundInterface
	{
		public:
			struct Config;

			SoundGroup(Config config);
			SoundGroup(const SoundGroup&) = delete;
			SoundGroup(SoundGroup&&) noexcept = default;
			~SoundGroup();

			SoundAttenuationModel GetAttenuationModel() const override;
			void GetCone(RadianAnglef& innerAngle, RadianAnglef& outerAngle, float& outerGain) const override;
			Vector3f GetDirection() const override;
			float GetDirectionalAttenuationFactor() const override;
			float GetDopplerFactor() const override;
			AudioEngine& GetEngine() override;
			const AudioEngine& GetEngine() const override;
			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;
			float GetMaxDistance() const override;
			float GetMaxGain() const override;
			float GetMinDistance() const override;
			float GetMinGain() const override;
			float GetPan() const override;
			SoundPanMode GetPanMode() const override;
			float GetPitch() const override;
			SoundPositioning GetPositioning() const override;
			float GetRolloff() const override;
			UInt32 GetListenerIndex() const override;
			Vector3f GetPosition() const override;
			Vector3f GetVelocity() const override;
			float GetVolume() const override;

			bool IsPlaying() const override;

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

			void Start();
			void Stop();

			SoundGroup& operator=(const SoundGroup&) = delete;
			SoundGroup& operator=(SoundGroup&&) noexcept = default;

			struct Config
			{
				AudioEngine* engine;
				AudioNode* node = nullptr;
			};

		private:
			MovableLiteral<std::size_t, MaxValue()> m_soundGroupIndex;
			MovablePtr<ma_sound_group> m_soundGroup;
	};
}

#include <Nazara/Audio/SoundGroup.inl>

#endif // NAZARA_AUDIO_SOUNDGROUP_HPP
