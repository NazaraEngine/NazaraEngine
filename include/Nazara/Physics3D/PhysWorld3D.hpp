// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSWORLD3D_HPP
#define NAZARA_PHYSICS3D_PHYSWORLD3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <NazaraUtils/MovablePtr.hpp>

class btDynamicsWorld;

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysWorld3D
	{
		public:
			PhysWorld3D();
			PhysWorld3D(const PhysWorld3D&) = delete;
			PhysWorld3D(PhysWorld3D&& ph) noexcept;
			~PhysWorld3D();

			btDynamicsWorld* GetDynamicsWorld();
			Vector3f GetGravity() const;
			std::size_t GetMaxStepCount() const;
			Time GetStepSize() const;

			void SetGravity(const Vector3f& gravity);
			void SetMaxStepCount(std::size_t maxStepCount);
			void SetStepSize(Time stepSize);

			void Step(Time timestep);

			PhysWorld3D& operator=(const PhysWorld3D&) = delete;
			PhysWorld3D& operator=(PhysWorld3D&&) noexcept;

		private:
			struct BulletWorld;

			std::size_t m_maxStepCount;
			std::unique_ptr<BulletWorld> m_world;
			Vector3f m_gravity;
			Time m_stepSize;
			Time m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSICS3D_PHYSWORLD3D_HPP
