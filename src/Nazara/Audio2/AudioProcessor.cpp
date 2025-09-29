// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio2/AudioProcessor.hpp>
#include <Nazara/Audio2/AudioEngine.hpp>
#include <Nazara/Core/Format.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <miniaudio.h>

namespace Nz
{
	struct AudioProcessor::MiniaudioNodeVTable : ma_node_vtable
	{
	};

	AudioProcessor::AudioProcessor(const Config& config)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		NazaraAssertMsg(config.engine, "invalid audio engine");

		m_vtable->onProcess = [](ma_node* node, const float** ppFramesIn, UInt32* frameCountIn, float** ppFramesOut, UInt32* frameCountOut)
		{
			AudioProcessor& processor = *static_cast<AudioProcessor*>(node);
			processor.OnProcess(ppFramesIn, frameCountIn, ppFramesOut, frameCountOut);
		};

		m_vtable->onGetRequiredInputFrameCount = nullptr;

		m_vtable->inputBusCount = config.inputBusCount;
		m_vtable->outputBusCount = config.outputBusCount;
		m_vtable->flags = 0;

		if (config.processWhenNoInput)
			m_vtable->flags |= MA_NODE_FLAG_CONTINUOUS_PROCESSING;

		StackArray<UInt32> defaultInputChannels;
		if (config.inputChannelCount.empty() && config.inputBusCount > 0)
		{
			UInt32 defaultChannelCount = config.engine->GetChannelCount();

			defaultInputChannels = NazaraStackArrayNoInit(UInt32, defaultChannelCount);
			for (UInt32 i = 0; i < config.inputBusCount; ++i)
				defaultInputChannels[i] = defaultChannelCount;
		}

		StackArray<UInt32> defaultOutputChannels;
		if (config.outputChannelCount.empty() && config.outputBusCount > 0)
		{
			UInt32 defaultChannelCount = config.engine->GetChannelCount();

			defaultOutputChannels = NazaraStackArrayNoInit(UInt32, defaultChannelCount);
			for (UInt32 i = 0; i < config.outputBusCount; ++i)
				defaultOutputChannels[i] = defaultChannelCount;
		}

		ma_node_config nodeConfig = ma_node_config_init();
		nodeConfig.pInputChannels = (!config.inputChannelCount.empty()) ? config.inputChannelCount.data() : defaultInputChannels.data();
		nodeConfig.pOutputChannels = (!config.outputChannelCount.empty()) ? config.outputChannelCount.data() : defaultOutputChannels.data();
		nodeConfig.vtable = m_vtable.Get();

		ma_result result = ma_node_init(ma_engine_get_node_graph(config.engine->GetInternalHandle()), &nodeConfig, nullptr, this);
		if (result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_node_init failed: {}", ma_result_description(result)));
	}

	AudioProcessor::~AudioProcessor()
	{
		ma_node_uninit(this, nullptr);
	}

	ma_node* AudioProcessor::GetInternalNode()
	{
		return this;
	}

	const ma_node* AudioProcessor::GetInternalNode() const
	{
		return this;
	}
}
