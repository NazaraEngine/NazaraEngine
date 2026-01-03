// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/AudioEffect.hpp>
#include <Nazara/Audio/AudioEngine.hpp>
#include <miniaudio.h>
#include <nodes/ma_reverb_node/ma_reverb_node.h>

namespace Nz
{
	struct AudioDelayEffect::MiniaudioDelayNode : ma_delay_node
	{
	};

	AudioDelayEffect::AudioDelayEffect(const Config& config)
	{
		NazaraAssertMsg(config.engine, "invalid audio engine");
		AudioEngine& engine = *config.engine;

		UInt32 channelCount = config.channelCount;
		if (channelCount == 0)
			channelCount = engine.GetChannelCount();

		UInt32 sampleRate = config.sampleRate;
		if (sampleRate == 0)
			sampleRate = engine.GetSampleRate();

		UInt32 delayInFrames = config.delayInFrames;
		if (delayInFrames == 0)
			delayInFrames = SafeCaster(config.decay * sampleRate);

		ma_delay_node_config delayNodeConfig = ma_delay_node_config_init(channelCount, sampleRate, delayInFrames, config.decay);
		delayNodeConfig.delay.decay = config.decay;
		delayNodeConfig.delay.dry = config.dry;
		delayNodeConfig.delay.wet = config.wet;

		ma_result result = ma_delay_node_init(engine.GetInternalGraph(), &delayNodeConfig, nullptr, m_soundNode.Get());
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_delay_node_init failed: {}", ma_result_description(result)));

		if (config.attachToNode)
		{
			if (config.outputNode)
				ma_node_attach_output_bus(&m_soundNode->baseNode, 0, config.outputNode->GetInternalNode(), config.outputNodeBus);
			else
				ma_node_attach_output_bus(&m_soundNode->baseNode, 0, engine.GetEndpoint().GetInternalNode(), 0);
		}
	}

	AudioDelayEffect::~AudioDelayEffect()
	{
		if (m_soundNode)
			ma_delay_node_uninit(m_soundNode.Get(), nullptr);
	}

	float AudioDelayEffect::GetDecay() const
	{
		return ma_delay_node_get_decay(m_soundNode.Get());
	}

	float AudioDelayEffect::GetDry() const
	{
		return ma_delay_node_get_dry(m_soundNode.Get());
	}

	float AudioDelayEffect::GetWet() const
	{
		return ma_delay_node_get_wet(m_soundNode.Get());
	}

	ma_node* AudioDelayEffect::GetInternalNode()
	{
		return &m_soundNode->baseNode;
	}

	const ma_node* AudioDelayEffect::GetInternalNode() const
	{
		return &m_soundNode->baseNode;
	}

	void AudioDelayEffect::SetDecay(float decay)
	{
		ma_delay_node_set_decay(m_soundNode.Get(), decay);
	}

	void AudioDelayEffect::SetDry(float dry)
	{
		ma_delay_node_set_decay(m_soundNode.Get(), dry);
	}

	void AudioDelayEffect::SetWet(float wet)
	{
		ma_delay_node_set_decay(m_soundNode.Get(), wet);
	}


	struct AudioReverbEffect::MiniaudioReverbNode : ma_reverb_node
	{
	};

	AudioReverbEffect::AudioReverbEffect(const Config& config)
	{
		NazaraAssertMsg(config.engine, "invalid audio engine");
		AudioEngine& engine = *config.engine;

		UInt32 channelCount = config.channelCount;
		if (channelCount == 0)
			channelCount = engine.GetChannelCount();

		UInt32 sampleRate = config.sampleRate;
		if (sampleRate == 0)
			sampleRate = engine.GetSampleRate();

		ma_reverb_node_config delayReverbConfig = ma_reverb_node_config_init(channelCount, sampleRate);
		delayReverbConfig.roomSize = config.roomSize;
		delayReverbConfig.damping = config.damping;
		delayReverbConfig.width = config.width;
		delayReverbConfig.wetVolume = config.wetVolume;
		delayReverbConfig.dryVolume = config.dryVolume;
		delayReverbConfig.mode = config.mode;

		ma_result result = ma_reverb_node_init(engine.GetInternalGraph(), &delayReverbConfig, nullptr, m_soundNode.Get());
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_reverb_node_init failed: {}", ma_result_description(result)));

		if (config.attachToNode)
		{
			if (config.outputNode)
				ma_node_attach_output_bus(&m_soundNode->baseNode, 0, config.outputNode->GetInternalNode(), config.outputNodeBus);
			else
				ma_node_attach_output_bus(&m_soundNode->baseNode, 0, engine.GetEndpoint().GetInternalNode(), 0);
		}
	}

	AudioReverbEffect::~AudioReverbEffect()
	{
		if (m_soundNode)
			ma_reverb_node_uninit(m_soundNode.Get(), nullptr);
	}

	ma_node* AudioReverbEffect::GetInternalNode()
	{
		return &m_soundNode->baseNode;
	}

	const ma_node* AudioReverbEffect::GetInternalNode() const
	{
		return &m_soundNode->baseNode;
	}


	struct AudioSplitter::MiniaudioSplitterNode : ma_splitter_node
	{
	};

	AudioSplitter::AudioSplitter(const Config& config)
	{
		NazaraAssertMsg(config.engine, "invalid audio engine");
		NazaraAssertMsg(config.outputBusCount > 0, "outputBusCount cannot be zero");

		AudioEngine& engine = *config.engine;

		UInt32 channelCount = config.channelCount;
		if (channelCount == 0)
			channelCount = engine.GetChannelCount();

		ma_splitter_node_config splitterNodeConfig = ma_splitter_node_config_init(channelCount);
		splitterNodeConfig.outputBusCount = config.outputBusCount;

		ma_result result = ma_splitter_node_init(engine.GetInternalGraph(), &splitterNodeConfig, nullptr, m_soundNode.Get());
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_splitter_node_init failed: {}", ma_result_description(result)));
	}

	AudioSplitter::~AudioSplitter()
	{
		if (m_soundNode)
			ma_splitter_node_uninit(m_soundNode.Get(), nullptr);
	}

	ma_node* AudioSplitter::GetInternalNode()
	{
		return &m_soundNode->base;
	}

	const ma_node* AudioSplitter::GetInternalNode() const
	{
		return &m_soundNode->base;
	}
}
