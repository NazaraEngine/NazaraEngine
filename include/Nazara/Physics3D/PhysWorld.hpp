// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD_HPP
#define NAZARA_PHYSWORLD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>

class NewtonWorld;

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysWorld
	{
		public:
			PhysWorld();
			PhysWorld(const PhysWorld&) = delete;
			PhysWorld(PhysWorld&&) = delete; ///TODO
			~PhysWorld();

			Vector3f GetGravity() const;
			NewtonWorld* GetHandle() const;
			float GetStepSize() const;

			void SetGravity(const Vector3f& gravity);
			void SetSolverModel(unsigned int model);
			void SetStepSize(float stepSize);

			void Step(float timestep);

			PhysWorld& operator=(const PhysWorld&) = delete;
			PhysWorld& operator=(PhysWorld&&) = delete; ///TODO

		private:
			Vector3f m_gravity;
			NewtonWorld* m_world;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD_HPP
