// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEQUENCE_HPP
#define NAZARA_SEQUENCE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

struct NzSequence
{
	NzString name;
	unsigned int firstFrame;
	unsigned int frameCount;
	unsigned int frameRate;
};

struct NzSequenceJoint
{
	NzQuaternionf rotation;
	NzVector3f position;
	NzVector3f scale;
};

#endif // NAZARA_SEQUENCE_HPP
