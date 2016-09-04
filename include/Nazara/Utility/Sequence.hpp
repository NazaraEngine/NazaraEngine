// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEQUENCE_HPP
#define NAZARA_SEQUENCE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct Sequence
	{
		String name;
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
