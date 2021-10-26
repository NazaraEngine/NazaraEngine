// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEQUENCE_HPP
#define NAZARA_SEQUENCE_HPP

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
		Quaternionf rotation;
		Vector3f position;
		Vector3f scale;
	};
}

#endif // NAZARA_SEQUENCE_HPP
