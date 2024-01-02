// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_SEQUENCE_HPP
#define NAZARA_UTILITY_SEQUENCE_HPP

#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct Sequence
	{
		std::string name;
		UInt32 firstFrame;
		UInt32 frameCount;
		UInt32 frameRate;
	};

	struct SequenceJoint
	{
		Quaternionf rotation = Quaternionf::Identity();
		Vector3f position = Vector3f::Zero();
		Vector3f scale = Vector3f::Unit();
	};
}

#endif // NAZARA_UTILITY_SEQUENCE_HPP
