// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLESTRUCT_HPP
#define NAZARA_PARTICLESTRUCT_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct ParticleStruct_Billboard
	{
		Color color;
		Vector3f normal;
		Vector3f position;
		Vector3f velocity;
		UInt32 life;
		float rotation;
	};

	struct ParticleStruct_Model
	{
		Vector3f position;
		Vector3f velocity;
		UInt32 life;
		Quaternionf rotation;
	};

	struct ParticleStruct_Sprite
	{
		Color color;
		Vector3f position;
		Vector3f velocity;
		UInt32 life;
		float rotation;
	};
}

#endif // NAZARA_PARTICLESTRUCT_HPP
