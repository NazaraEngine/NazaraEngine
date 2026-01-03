// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIOPROCESSOR_HPP
#define NAZARA_AUDIO_AUDIOPROCESSOR_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/AudioNode.hpp>
#include <Nazara/Audio/Export.hpp>
#include <NazaraUtils/PrivateImpl.hpp>
#include <span>

struct ma_node_base;

namespace Nz
{
	class AudioEngine;

	class NAZARA_AUDIO_API AudioProcessor : public AudioNode
	{
		public:
			struct Config;

			AudioProcessor(const Config& config);
			AudioProcessor(const AudioProcessor&) = delete;
			AudioProcessor(AudioProcessor&&) = delete;
			~AudioProcessor();

			ma_node* GetInternalNode() override;
			const ma_node* GetInternalNode() const override;

			virtual void OnProcess(const float** ppFramesIn, UInt32* frameCountIn, float** ppFramesOut, UInt32* frameCountOut) = 0;

			AudioProcessor& operator=(const AudioProcessor&) = delete;
			AudioProcessor& operator=(AudioProcessor&&) = delete;

			struct Config
			{
				AudioEngine* engine = nullptr; //< mandatory

				std::span<const UInt32> inputChannelCount; //< empty => array of inputBusCount filled with engine channel count
				std::span<const UInt32> outputChannelCount; //< empty => array of outputBusCount filled with engine channel count
				UInt8 inputBusCount = 1;
				UInt8 outputBusCount = 1;
				bool processWhenNoInput = false;
			};

		private:
			struct MiniaudioNodeVTable;

			PrivateImpl<ma_node_base, 360, 8> m_nodeBase; //< must be the first member
			PrivateImpl<MiniaudioNodeVTable, 24, 8> m_vtable;
	};
}

#include <Nazara/Audio/AudioProcessor.inl>

#endif // NAZARA_AUDIO_AUDIOPROCESSOR_HPP
