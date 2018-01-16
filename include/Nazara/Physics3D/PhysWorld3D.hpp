// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD_HPP
#define NAZARA_PHYSWORLD_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>

class NewtonWorld;

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysWorld3D
	{
		public:
			PhysWorld3D();
			PhysWorld3D(const PhysWorld3D&) = delete;
			PhysWorld3D(PhysWorld3D&&) = delete; ///TODO
			~PhysWorld3D();

			Vector3f GetGravity() const;
			NewtonWorld* GetHandle() const;
			float GetStepSize() const;

			void SetGravity(const Vector3f& gravity);
			void SetSolverModel(unsigned int model);
			void SetStepSize(float stepSize);

			void Step(float timestep);

			PhysWorld3D& operator=(const PhysWorld3D&) = delete;
			PhysWorld3D& operator=(PhysWorld3D&&) = delete; ///TODO

		private:
			Vector3f m_gravity;
			NewtonWorld* m_world;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD_HPP
