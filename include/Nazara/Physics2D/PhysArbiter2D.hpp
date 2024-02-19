// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_PHYSARBITER2D_HPP
#define NAZARA_PHYSICS2D_PHYSARBITER2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Export.hpp>
#include <NazaraUtils/MovablePtr.hpp>

struct cpArbiter;

namespace Nz
{
	class RigidBody2D;

	class NAZARA_PHYSICS2D_API PhysArbiter2D
	{
		public:
			inline PhysArbiter2D(cpArbiter* arbiter);
			PhysArbiter2D(const PhysArbiter2D&) = delete;
			PhysArbiter2D(PhysArbiter2D&&) = default;
			~PhysArbiter2D() = default;

			float ComputeTotalKinematicEnergy() const;
			Nz::Vector2f ComputeTotalImpulse() const;

			std::pair<RigidBody2D*, RigidBody2D*> GetBodies() const;

			std::size_t GetContactCount() const;
			float GetContactDepth(std::size_t i) const;
			Vector2f GetContactPointA(std::size_t i) const;
			Vector2f GetContactPointB(std::size_t i) const;

			float GetElasticity() const;
			float GetFriction() const;
			Vector2f GetNormal() const;
			Vector2f GetSurfaceVelocity() const;

			bool IsFirstContact() const;
			bool IsRemoval() const;

			void SetElasticity(float elasticity);
			void SetFriction(float friction);
			void SetSurfaceVelocity(const Vector2f& surfaceVelocity);

			PhysArbiter2D& operator=(const PhysArbiter2D&) = delete;
			PhysArbiter2D& operator=(PhysArbiter2D&&) = default;

		private:
			MovablePtr<cpArbiter> m_arbiter;
	};
}

#include <Nazara/Physics2D/PhysArbiter2D.inl>

#endif // NAZARA_PHYSICS2D_PHYSARBITER2D_HPP
