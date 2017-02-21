// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD2D_HPP
#define NAZARA_PHYSWORLD2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <memory>
#include <unordered_map>

struct cpCollisionHandler;
struct cpSpace;

namespace Nz
{
	class RigidBody2D;

	class NAZARA_PHYSICS2D_API PhysWorld2D
	{
		using ContactEndCallback = void(*)(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata);
		using ContactPreSolveCallback = bool(*)(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata);
		using ContactPostSolveCallback = void(*)(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata);
		using ContactStartCallback = bool(*)(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata);

		public:
			struct Callback;

			PhysWorld2D();
			PhysWorld2D(const PhysWorld2D&) = delete;
			PhysWorld2D(PhysWorld2D&&) = delete; ///TODO
			~PhysWorld2D();

			Vector2f GetGravity() const;
			cpSpace* GetHandle() const;
			float GetStepSize() const;

			void RegisterCallbacks(unsigned int collisionId, const Callback& callbacks);
			void RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, const Callback& callbacks);

			void SetGravity(const Vector2f& gravity);
			void SetStepSize(float stepSize);

			void Step(float timestep);

			PhysWorld2D& operator=(const PhysWorld2D&) = delete;
			PhysWorld2D& operator=(PhysWorld2D&&) = delete; ///TODO

			struct Callback
			{
				ContactEndCallback endCallback;
				ContactPreSolveCallback preSolveCallback;
				ContactPostSolveCallback postSolveCallback;
				ContactStartCallback startCallback;
				void* userdata;
			};

		private:
			void InitCallbacks(cpCollisionHandler* handler, const Callback& callbacks);

			std::unordered_map<cpCollisionHandler*, std::unique_ptr<Callback>> m_callbacks;
			cpSpace* m_handle;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD2D_HPP
