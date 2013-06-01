// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD_HPP
#define NAZARA_PHYSWORLD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Vector3.hpp>

struct NewtonWorld;

class NAZARA_API NzPhysWorld : NzNonCopyable
{
	public:
		NzPhysWorld();
		~NzPhysWorld();

		NzVector3f GetGravity() const;
		NewtonWorld* GetHandle() const;
		unsigned int GetMemoryUsed() const;

		void SetGravity(const NzVector3f& gravity);
		void SetSize(const NzCubef& cube);
		void SetSize(const NzVector3f& min, const NzVector3f& max);
		void SetSolverModel(unsigned int model);

		void Update(float timestep);

	private:
		NzVector3f m_gravity;
		NewtonWorld* m_world;
};

#endif // NAZARA_PHYSWORLD_HPP
