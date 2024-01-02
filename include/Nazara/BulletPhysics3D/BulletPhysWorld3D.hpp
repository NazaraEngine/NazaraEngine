// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_BULLETPHYSWORLD3D_HPP
#define NAZARA_BULLETPHYSICS3D_BULLETPHYSWORLD3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <optional>

class btDynamicsWorld;
class btRigidBody;

namespace Nz
{
	class BulletRigidBody3D;

	class NAZARA_BULLETPHYSICS3D_API BulletPhysWorld3D
	{
		friend BulletRigidBody3D;

		public:
			struct RaycastHit;

			BulletPhysWorld3D();
			BulletPhysWorld3D(const BulletPhysWorld3D&) = delete;
			BulletPhysWorld3D(BulletPhysWorld3D&& ph) = delete;
			~BulletPhysWorld3D();

			btDynamicsWorld* GetDynamicsWorld();
			Vector3f GetGravity() const;
			std::size_t GetMaxStepCount() const;
			Time GetStepSize() const;

			bool RaycastQuery(const Vector3f& from, const Vector3f& to, const FunctionRef<std::optional<float>(const RaycastHit& hitInfo)>& callback);
			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo = nullptr);

			void SetGravity(const Vector3f& gravity);
			void SetMaxStepCount(std::size_t maxStepCount);
			void SetStepSize(Time stepSize);

			void Step(Time timestep);

			BulletPhysWorld3D& operator=(const BulletPhysWorld3D&) = delete;
			BulletPhysWorld3D& operator=(BulletPhysWorld3D&&) = delete;

			struct RaycastHit
			{
				float fraction;
				BulletRigidBody3D* hitBody = nullptr;
				Vector3f hitPosition;
				Vector3f hitNormal;
			};

		private:
			btRigidBody* AddRigidBody(std::size_t& rigidBodyIndex, FunctionRef<void(btRigidBody* body)> constructor);
			void RemoveRigidBody(btRigidBody* rigidBody, std::size_t rigidBodyIndex);

			struct BulletWorld;

			std::size_t m_maxStepCount;
			std::unique_ptr<BulletWorld> m_world;
			Vector3f m_gravity;
			Time m_stepSize;
			Time m_timestepAccumulator;
	};
}

#endif // NAZARA_BULLETPHYSICS3D_BULLETPHYSWORLD3D_HPP
