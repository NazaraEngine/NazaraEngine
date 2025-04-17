// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Audio2 module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_AUDIO2_AUDIONODE_HPP
#define NAZARA_AUDIO2_AUDIONODE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Audio2/Export.hpp>

using ma_node = void;

namespace Nz
{
	class NAZARA_AUDIO2_API AudioNode
	{
		public:
			AudioNode() = default;
			AudioNode(const AudioNode&) = delete;
			AudioNode(AudioNode&&) = delete;
			virtual ~AudioNode();

			virtual ma_node* GetInternalNode() = 0;

			AudioNode& operator=(const AudioNode&) = delete;
			AudioNode& operator=(AudioNode&&) = delete;
	};
}

#include <Nazara/Audio2/AudioNode.inl>

#endif // NAZARA_AUDIO2_AUDIONODE_HPP
