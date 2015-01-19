// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLESTRUCT_HPP
#define NAZARA_PARTICLESTRUCT_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>

struct NzParticleStruct_Billboard
{
	NzColor color;
	NzVector3f normal;
	NzVector3f position;
	NzVector3f velocity;
	nzUInt32 life;
	float rotation;
};

struct NzParticleStruct_Model
{
	NzVector3f position;
	NzVector3f velocity;
	nzUInt32 life;
	NzQuaternionf rotation;
};

struct NzParticleStruct_Sprite
{
	NzColor color;
	NzVector2f position;
	NzVector2f velocity;
	nzUInt32 life;
	float rotation;
};

#endif // NAZARA_PARTICLESTRUCT_HPP
