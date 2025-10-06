// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Audio/AudioNode.hpp>
#include <Nazara/Core/Format.hpp>
#include <miniaudio.h>

namespace Nz
{
	AudioNode::~AudioNode() = default;

	void AudioNode::AttachOutputBus(UInt32 outputBusIndex, AudioNode& otherNode, UInt32 otherNodeInputIndex)
	{
		ma_result result = ma_node_attach_output_bus(GetInternalNode(), outputBusIndex, otherNode.GetInternalNode(), otherNodeInputIndex);
		if NAZARA_UNLIKELY(result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_node_attach_output_bus failed: {}", ma_result_description(result)));
	}

	void AudioNode::DetachOutputBus(UInt32 outputBusIndex)
	{
		ma_result result = ma_node_detach_output_bus(GetInternalNode(), outputBusIndex);
		if NAZARA_UNLIKELY(result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_node_attach_output_bus failed: {}", ma_result_description(result)));
	}

	void AudioNode::DetachAllOutputBuses()
	{
		ma_result result = ma_node_detach_all_output_buses(GetInternalNode());
		if NAZARA_UNLIKELY(result != MA_SUCCESS)
			throw std::runtime_error(Format("ma_node_detach_all_output_buses failed: {}", ma_result_description(result)));
	}

	UInt32 AudioNode::GetInputBusCount() const
	{
		return ma_node_get_input_bus_count(GetInternalNode());
	}

	UInt32 AudioNode::GetInputChannelCount(UInt32 inputBusIndex) const
	{
		return ma_node_get_input_channels(GetInternalNode(), inputBusIndex);
	}

	UInt32 AudioNode::GetOutputBusCount() const
	{
		return ma_node_get_output_bus_count(GetInternalNode());
	}

	float AudioNode::GetOutputBusVolume(UInt32 outputBusIndex) const
	{
		return ma_node_get_output_bus_volume(GetInternalNode(), outputBusIndex);
	}

	UInt32 AudioNode::GetOutputChannelCount(UInt32 outputBusIndex) const
	{
		return ma_node_get_output_channels(GetInternalNode(), outputBusIndex);
	}

	void AudioNode::SetOutputBusVolume(UInt32 outputBusIndex, float volume)
	{
		ma_node_set_output_bus_volume(GetInternalNode(), outputBusIndex, volume);
	}
}
