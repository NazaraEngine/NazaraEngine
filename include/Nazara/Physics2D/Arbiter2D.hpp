// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ARBITER2D_HPP
#define NAZARA_ARBITER2D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Config.hpp>

struct cpArbiter;

namespace Nz
{
	class NAZARA_PHYSICS2D_API Arbiter2D
	{
		public:
			inline Arbiter2D(cpArbiter* arbiter);
			Arbiter2D(const Arbiter2D&) = delete;
			Arbiter2D(Arbiter2D&&) = default;
			~Arbiter2D() = default;

			float ComputeTotalKinematicEnergy() const;
			Nz::Vector2f ComputeTotalImpulse() const;

			std::size_t GetContactCount() const;
			float GetContactDepth(std::size_t i) const;
			Nz::Vector2f GetContactPointA(std::size_t i) const;
			Nz::Vector2f GetContactPointB(std::size_t i) const;

			float GetElasticity() const;
			float GetFriction() const;
			Nz::Vector2f GetNormal() const;
			Nz::Vector2f GetSurfaceVelocity() const;

			bool IsFirstContact() const;
			bool IsRemoval() const;

			void SetElasticity(float elasticity);
			void SetFriction(float friction);
			void SetSurfaceVelocity(const Nz::Vector2f& surfaceVelocity);

			Arbiter2D& operator=(const Arbiter2D&) = delete;
			Arbiter2D& operator=(Arbiter2D&&) = default;

		private:
			MovablePtr<cpArbiter> m_arbiter;
	};
}

#include <Nazara/Physics2D/Arbiter2D.inl>

#endif // NAZARA_ARBITER2D_HPP
