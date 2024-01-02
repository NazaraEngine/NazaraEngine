// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/BulletRigidBody3D.hpp>
#include <Nazara/BulletPhysics3D/BulletHelper.hpp>
#include <Nazara/BulletPhysics3D/BulletPhysWorld3D.hpp>
#include <NazaraUtils/MemoryHelper.hpp>
#include <BulletDynamics/Dynamics/btDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	BulletRigidBody3D::BulletRigidBody3D(BulletPhysWorld3D* world, const Matrix4f& mat) :
	BulletRigidBody3D(world, nullptr, mat)
	{
	}

	BulletRigidBody3D::BulletRigidBody3D(BulletPhysWorld3D* world, std::shared_ptr<BulletCollider3D> geom, const Matrix4f& mat) :
	m_geom(std::move(geom)),
	m_world(world)
	{
		NazaraAssert(m_world, "Invalid world");

		if (!m_geom)
			m_geom = std::make_shared<BulletNullCollider3D>();

		Vector3f inertia;
		m_geom->ComputeInertia(1.f, &inertia);

		btRigidBody::btRigidBodyConstructionInfo constructionInfo(1.f, nullptr, m_geom->GetShape(), ToBullet(inertia));
		constructionInfo.m_startWorldTransform = ToBullet(mat);

		m_body = m_world->AddRigidBody(m_bodyPoolIndex, [&](btRigidBody* body)
		{
			PlacementNew(body, constructionInfo);
		});
		m_body->setUserPointer(this);
	}

	BulletRigidBody3D::BulletRigidBody3D(BulletRigidBody3D&& object) noexcept :
	m_geom(std::move(object.m_geom)),
	m_bodyPoolIndex(object.m_bodyPoolIndex),
	m_body(object.m_body),
	m_world(object.m_world)
	{
		if (m_body)
			m_body->setUserPointer(this);

		object.m_body = nullptr;
	}

	BulletRigidBody3D::~BulletRigidBody3D()
	{
		Destroy();
	}

	void BulletRigidBody3D::AddForce(const Vector3f& force, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				WakeUp();
				m_body->applyCentralForce(ToBullet(force));
				break;

			case CoordSys::Local:
				WakeUp();
				m_body->applyCentralForce(ToBullet(GetRotation() * force));
				break;
		}
	}

	void BulletRigidBody3D::AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				WakeUp();
				m_body->applyForce(ToBullet(force), ToBullet(point));
				break;

			case CoordSys::Local:
			{
				Matrix4f transformMatrix = GetMatrix();
				return AddForce(transformMatrix.Transform(force, 0.f), point, CoordSys::Global);
			}
		}
	}

	void BulletRigidBody3D::AddTorque(const Vector3f& torque, CoordSys coordSys)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				WakeUp();
				m_body->applyTorque(ToBullet(torque));
				break;

			case CoordSys::Local:
				Matrix4f transformMatrix = GetMatrix();
				WakeUp();
				m_body->applyTorque(ToBullet(transformMatrix.Transform(torque, 0.f)));
				break;
		}
	}

	void BulletRigidBody3D::EnableSleeping(bool enable)
	{
		if (enable)
		{
			if (m_body->getActivationState() == DISABLE_DEACTIVATION)
				m_body->setActivationState(ACTIVE_TAG);
		}
		else
		{
			if (m_body->getActivationState() != DISABLE_DEACTIVATION)
				m_body->setActivationState(DISABLE_DEACTIVATION);
		}
	}

	void BulletRigidBody3D::FallAsleep()
	{
		if (m_body->getActivationState() != DISABLE_DEACTIVATION)
			m_body->setActivationState(ISLAND_SLEEPING);
	}

	Boxf BulletRigidBody3D::GetAABB() const
	{
		btVector3 min, max;
		m_body->getAabb(min, max);

		return Boxf(FromBullet(min), FromBullet(max));
	}

	float BulletRigidBody3D::GetAngularDamping() const
	{
		return m_body->getAngularDamping();
	}

	Vector3f BulletRigidBody3D::GetAngularVelocity() const
	{
		return FromBullet(m_body->getAngularVelocity());
	}

	float BulletRigidBody3D::GetLinearDamping() const
	{
		return m_body->getLinearDamping();
	}

	Vector3f BulletRigidBody3D::GetLinearVelocity() const
	{
		return FromBullet(m_body->getLinearVelocity());
	}

	float BulletRigidBody3D::GetMass() const
	{
		return m_body->getMass();
	}

	Vector3f BulletRigidBody3D::GetMassCenter(CoordSys coordSys) const
	{
		return FromBullet(m_body->getCenterOfMassPosition());
	}

	Matrix4f BulletRigidBody3D::GetMatrix() const
	{
		return FromBullet(m_body->getWorldTransform());
	}

	Vector3f BulletRigidBody3D::GetPosition() const
	{
		return FromBullet(m_body->getWorldTransform().getOrigin());
	}

	Quaternionf BulletRigidBody3D::GetRotation() const
	{
		return FromBullet(m_body->getWorldTransform().getRotation());
	}

	bool BulletRigidBody3D::IsSimulationEnabled() const
	{
		return m_body->isActive();
	}

	bool BulletRigidBody3D::IsSleeping() const
	{
		return m_body->getActivationState() == ISLAND_SLEEPING;
	}

	bool BulletRigidBody3D::IsSleepingEnabled() const
	{
		return m_body->getActivationState() != DISABLE_DEACTIVATION;
	}

	void BulletRigidBody3D::SetAngularDamping(float angularDamping)
	{
		m_body->setDamping(m_body->getLinearDamping(), angularDamping);
	}

	void BulletRigidBody3D::SetAngularVelocity(const Vector3f& angularVelocity)
	{
		m_body->setAngularVelocity(ToBullet(angularVelocity));
	}

	void BulletRigidBody3D::SetGeom(std::shared_ptr<BulletCollider3D> geom, bool recomputeInertia)
	{
		if (m_geom != geom)
		{
			if (geom)
				m_geom = std::move(geom);
			else
				m_geom = std::make_shared<BulletNullCollider3D>();

			m_body->setCollisionShape(m_geom->GetShape());
			if (recomputeInertia)
			{
				float mass = GetMass();

				Vector3f inertia;
				m_geom->ComputeInertia(mass, &inertia);

				m_body->setMassProps(mass, ToBullet(inertia));
			}
		}
	}

	void BulletRigidBody3D::SetLinearDamping(float damping)
	{
		m_body->setDamping(damping, m_body->getAngularDamping());
	}

	void BulletRigidBody3D::SetLinearVelocity(const Vector3f& velocity)
	{
		m_body->setLinearVelocity(ToBullet(velocity));
	}

	void BulletRigidBody3D::SetMass(float mass)
	{
		NazaraAssert(mass >= 0.f, "Mass must be positive and finite");
		NazaraAssert(std::isfinite(mass), "Mass must be positive and finite");

		Vector3f inertia;
		m_geom->ComputeInertia(mass, &inertia);

		m_body->setMassProps(mass, ToBullet(inertia));
	}

	void BulletRigidBody3D::SetMassCenter(const Vector3f& center)
	{
		btTransform centerTransform;
		centerTransform.setIdentity();
		centerTransform.setOrigin(ToBullet(center));

		m_body->setCenterOfMassTransform(centerTransform);
	}

	void BulletRigidBody3D::SetPosition(const Vector3f& position)
	{
		btTransform worldTransform = m_body->getWorldTransform();
		worldTransform.setOrigin(ToBullet(position));

		m_body->setWorldTransform(worldTransform);
	}

	void BulletRigidBody3D::SetPositionAndRotation(const Vector3f& position, const Quaternionf& rotation)
	{
		btTransform worldTransform;
		worldTransform.setOrigin(ToBullet(position));
		worldTransform.setRotation(ToBullet(rotation));

		m_body->setWorldTransform(worldTransform);
	}

	void BulletRigidBody3D::SetRotation(const Quaternionf& rotation)
	{
		btTransform worldTransform = m_body->getWorldTransform();
		worldTransform.setRotation(ToBullet(rotation));

		m_body->setWorldTransform(worldTransform);
	}

	Quaternionf BulletRigidBody3D::ToLocal(const Quaternionf& worldRotation)
	{
		return GetRotation().Conjugate() * worldRotation;
	}

	Vector3f BulletRigidBody3D::ToLocal(const Vector3f& worldPosition)
	{
		return GetMatrix().InverseTransform() * worldPosition;
	}

	Quaternionf BulletRigidBody3D::ToWorld(const Quaternionf& localRotation)
	{
		return GetRotation() * localRotation;
	}

	Vector3f BulletRigidBody3D::ToWorld(const Vector3f& localPosition)
	{
		return GetMatrix() * localPosition;
	}

	void BulletRigidBody3D::WakeUp()
	{
		m_body->activate();
	}

	BulletRigidBody3D& BulletRigidBody3D::operator=(BulletRigidBody3D&& object) noexcept
	{
		Destroy();

		m_body          = object.m_body;
		m_bodyPoolIndex = object.m_bodyPoolIndex;
		m_geom          = std::move(object.m_geom);
		m_world         = object.m_world;

		if (m_body)
			m_body->setUserPointer(this);

		object.m_body = nullptr;

		return *this;
	}

	void BulletRigidBody3D::Destroy()
	{
		if (m_body)
		{
			m_world->RemoveRigidBody(m_body, m_bodyPoolIndex);
			m_body = nullptr;
		}

		m_geom.reset();
	}
}
