// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_SOUNDEFFECT_HPP
#define NAZARA_AUDIO2_SOUNDEFFECT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/PrivateImpl.hpp>
#include <Nazara/Audio2/Export.hpp>
#include <Nazara/Audio2/AudioNode.hpp>

namespace Nz
{
	class AudioEngine;

	class NAZARA_AUDIO2_API AudioEffect : public AudioNode
	{
	};

	class NAZARA_AUDIO2_API AudioDelayEffect final : public AudioEffect
	{
		public:
			struct Config;

			AudioDelayEffect(const Config& config);
			AudioDelayEffect(const AudioDelayEffect&) = delete;
			AudioDelayEffect(AudioDelayEffect&&) = default;
			~AudioDelayEffect();

			float GetDecay() const;
			float GetDry() const;
			float GetWet() const;

			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;

			void SetDecay(float decay);
			void SetDry(float dry);
			void SetWet(float wet);

			AudioDelayEffect& operator=(const AudioDelayEffect&) = delete;
			AudioDelayEffect& operator=(AudioDelayEffect&&) = default;

			struct Config
			{
				AudioEngine* engine = nullptr; //< mandatory
				AudioNode* outputNode = nullptr; //< nullptr => engine endpoint
				UInt32 outputNodeBus = 0;
				bool attachToNode = true;

				// Delay config
				UInt32 channelCount = 0; //< 0 => engine channels
				UInt32 delayInFrames = 0; //< if not zero this is used instead of decay
				UInt32 sampleRate = 0; //< 0 => engine sample rate
				float decay = 0.5f;
				float delay = 0.2f; //< in seconds
				float wet = 1.0f;
				float dry = 1.0f;
			};

		private:
			struct MiniaudioDelayNode; // ma_delay_node

			PrivateImpl<MiniaudioDelayNode> m_soundNode;
	};

	class NAZARA_AUDIO2_API AudioReverbEffect final : public AudioEffect
	{
		public:
			struct Config;

			AudioReverbEffect(const Config& config);
			AudioReverbEffect(const AudioReverbEffect&) = delete;
			AudioReverbEffect(AudioReverbEffect&&) = default;
			~AudioReverbEffect();

			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;

			AudioReverbEffect& operator=(const AudioReverbEffect&) = delete;
			AudioReverbEffect& operator=(AudioReverbEffect&&) = default;

			struct Config
			{
				AudioEngine* engine = nullptr; //< mandatory
				AudioNode* outputNode = nullptr; //< nullptr => engine endpoint
				UInt32 outputNodeBus = 0;
				bool attachToNode = true;

				// Delay config
				UInt32 channelCount = 0; //< 0 => engine channels
				UInt32 sampleRate = 0; //< 0 => engine sample rate
				float roomSize = 0.5f;
				float damping = 0.25f;
				float width = 1.0f;
				float wetVolume = 1.0f / 3.0f;
				float dryVolume = 0.0f;
				float mode = 0.0f;
			};

		private:
			struct MiniaudioReverbNode; // ma_reverb_node

			PrivateImpl<MiniaudioReverbNode> m_soundNode;
	};

	class NAZARA_AUDIO2_API AudioSplitter final : public AudioEffect
	{
		public:
			struct Config;

			AudioSplitter(const Config& config);
			AudioSplitter(const AudioSplitter&) = delete;
			AudioSplitter(AudioSplitter&&) = default;
			~AudioSplitter();

			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;

			AudioSplitter& operator=(const AudioSplitter&) = delete;
			AudioSplitter& operator=(AudioSplitter&&) = default;

			struct Config
			{
				AudioEngine* engine = nullptr; //< mandatory

				// Splitter config
				UInt32 outputBusCount = 2;
				UInt32 channelCount = 0; //< 0 => engine channels
			};

		private:
			struct MiniaudioSplitterNode; // ma_splitter_node

			PrivateImpl<MiniaudioSplitterNode> m_soundNode;
	};
}

#include <Nazara/Audio2/AudioEffect.inl>

#endif // NAZARA_AUDIO2_SOUNDEFFECT_HPP
