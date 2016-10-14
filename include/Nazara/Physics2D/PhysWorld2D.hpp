// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD2D_HPP
#define NAZARA_PHYSWORLD2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Config.hpp>

struct cpSpace;

namespace Nz
{
	class NAZARA_PHYSICS2D_API PhysWorld2D
	{
		public:
			PhysWorld2D();
			PhysWorld2D(const PhysWorld2D&) = delete;
			PhysWorld2D(PhysWorld2D&&) = delete; ///TODO
			~PhysWorld2D();

			Vector2f GetGravity() const;
			cpSpace* GetHandle() const;
			float GetStepSize() const;

			void SetGravity(const Vector2f& gravity);
			void SetSolverModel(unsigned int model);
			void SetStepSize(float stepSize);

			void Step(float timestep);

			PhysWorld2D& operator=(const PhysWorld2D&) = delete;
			PhysWorld2D& operator=(PhysWorld2D&&) = delete; ///TODO

		private:
			cpSpace* m_handle;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD2D_HPP
