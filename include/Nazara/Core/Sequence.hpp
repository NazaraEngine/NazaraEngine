// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SEQUENCE_HPP
#define NAZARA_CORE_SEQUENCE_HPP

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

#endif // NAZARA_CORE_SEQUENCE_HPP
