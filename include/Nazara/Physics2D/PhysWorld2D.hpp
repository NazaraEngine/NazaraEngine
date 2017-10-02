// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD2D_HPP
#define NAZARA_PHYSWORLD2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

struct cpCollisionHandler;
struct cpSpace;

namespace Nz
{
	class NAZARA_PHYSICS2D_API PhysWorld2D
	{
		friend RigidBody2D;

		using ContactEndCallback = std::function<void(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)>;
		using ContactPreSolveCallback = std::function<bool(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)>;
		using ContactPostSolveCallback = std::function<void(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)>;
		using ContactStartCallback = std::function<bool(PhysWorld2D& world, RigidBody2D& bodyA, RigidBody2D& bodyB, void* userdata)>;

		public:
			struct Callback;
			struct NearestQueryResult;
			struct RaycastHit;

			PhysWorld2D();
			PhysWorld2D(const PhysWorld2D&) = delete;
			PhysWorld2D(PhysWorld2D&&) = delete; ///TODO
			~PhysWorld2D();

			float GetDamping() const;
			Vector2f GetGravity() const;
			cpSpace* GetHandle() const;
			float GetStepSize() const;

			bool NearestBodyQuery(const Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, RigidBody2D** nearestBody = nullptr);
			bool NearestBodyQuery(const Vector2f& from, float maxDistance, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, NearestQueryResult* result);

			bool RaycastQuery(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<RaycastHit>* hitInfos);
			bool RaycastQueryFirst(const Nz::Vector2f& from, const Nz::Vector2f& to, float radius, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, RaycastHit* hitInfo = nullptr);

			void RegionQuery(const Nz::Rectf& boundingBox, Nz::UInt32 collisionGroup, Nz::UInt32 categoryMask, Nz::UInt32 collisionMask, std::vector<Nz::RigidBody2D*>* bodies);

			void RegisterCallbacks(unsigned int collisionId, const Callback& callbacks);
			void RegisterCallbacks(unsigned int collisionIdA, unsigned int collisionIdB, const Callback& callbacks);

			void SetDamping(float dampingValue);
			void SetGravity(const Vector2f& gravity);
			void SetStepSize(float stepSize);

			void Step(float timestep);

			PhysWorld2D& operator=(const PhysWorld2D&) = delete;
			PhysWorld2D& operator=(PhysWorld2D&&) = delete; ///TODO

			struct Callback
			{
				ContactEndCallback endCallback = nullptr;
				ContactPreSolveCallback preSolveCallback = nullptr;
				ContactPostSolveCallback postSolveCallback = nullptr;
				ContactStartCallback startCallback = nullptr;
				void* userdata;
			};

			struct NearestQueryResult
			{
				Nz::RigidBody2D* nearestBody;
				Nz::Vector2f closestPoint;
				Nz::Vector2f fraction;
				float distance;
			};

			struct RaycastHit
			{
				Nz::RigidBody2D* nearestBody;
				Nz::Vector2f hitPos;
				Nz::Vector2f hitNormal;
				float fraction;
			};

			NazaraSignal(OnPhysWorld2DPreStep, const PhysWorld2D* /*physWorld*/);
			NazaraSignal(OnPhysWorld2DPostStep, const PhysWorld2D* /*physWorld*/);

		private:
			void InitCallbacks(cpCollisionHandler* handler, const Callback& callbacks);

			using PostStep = std::function<void(Nz::RigidBody2D* body)>;

			void OnRigidBodyMoved(RigidBody2D* oldPointer, RigidBody2D* newPointer);
			void OnRigidBodyRelease(RigidBody2D* rigidBody);

			void RegisterPostStep(RigidBody2D* rigidBody, PostStep&& func);

			struct PostStepContainer
			{
				NazaraSlot(RigidBody2D, OnRigidBody2DMove, onMovedSlot);
				NazaraSlot(RigidBody2D, OnRigidBody2DRelease, onReleaseSlot);

				std::vector<PostStep> funcs;
			};

			static_assert(std::is_nothrow_move_constructible<PostStepContainer>::value, "PostStepContainer should be noexcept MoveConstructible");

			std::unordered_map<cpCollisionHandler*, std::unique_ptr<Callback>> m_callbacks;
			std::unordered_map<RigidBody2D*, PostStepContainer> m_rigidPostSteps;
			cpSpace* m_handle;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD2D_HPP
