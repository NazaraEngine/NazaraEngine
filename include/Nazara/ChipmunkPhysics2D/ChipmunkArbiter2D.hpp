// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKARBITER2D_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKARBITER2D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/Config.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <NazaraUtils/MovablePtr.hpp>

struct cpArbiter;

namespace Nz
{
	class ChipmunkRigidBody2D;

	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkArbiter2D
	{
		public:
			inline ChipmunkArbiter2D(cpArbiter* arbiter);
			ChipmunkArbiter2D(const ChipmunkArbiter2D&) = delete;
			ChipmunkArbiter2D(ChipmunkArbiter2D&&) = default;
			~ChipmunkArbiter2D() = default;

			float ComputeTotalKinematicEnergy() const;
			Nz::Vector2f ComputeTotalImpulse() const;

			std::pair<ChipmunkRigidBody2D*, ChipmunkRigidBody2D*> GetBodies() const;

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

			ChipmunkArbiter2D& operator=(const ChipmunkArbiter2D&) = delete;
			ChipmunkArbiter2D& operator=(ChipmunkArbiter2D&&) = default;

		private:
			MovablePtr<cpArbiter> m_arbiter;
	};
}

#include <Nazara/ChipmunkPhysics2D/ChipmunkArbiter2D.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKARBITER2D_HPP
