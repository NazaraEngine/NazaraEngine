// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO_AUDIONODE_HPP
#define NAZARA_AUDIO_AUDIONODE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio/Export.hpp>

using ma_node = void;

namespace Nz
{
	class NAZARA_AUDIO_API AudioNode
	{
		public:
			AudioNode() = default;
			AudioNode(const AudioNode&) = delete;
			AudioNode(AudioNode&&) = default;
			virtual ~AudioNode();

			void AttachOutputBus(UInt32 outputBusIndex, AudioNode& otherNode, UInt32 otherNodeInputIndex);
			void DetachOutputBus(UInt32 outputBusIndex);
			void DetachAllOutputBuses();

			UInt32 GetInputBusCount() const;
			UInt32 GetInputChannelCount(UInt32 inputBusIndex) const;

			virtual ma_node* GetInternalNode() = 0;
			virtual const ma_node* GetInternalNode() const = 0;

			UInt32 GetOutputBusCount() const;
			float GetOutputBusVolume(UInt32 outputBusIndex) const;
			UInt32 GetOutputChannelCount(UInt32 outputBusIndex) const;

			void SetOutputBusVolume(UInt32 outputBusIndex, float volume);

			AudioNode& operator=(const AudioNode&) = delete;
			AudioNode& operator=(AudioNode&&) = default;
	};
}

#include <Nazara/Audio/AudioNode.inl>

#endif // NAZARA_AUDIO_AUDIONODE_HPP
