// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Components/CollisionComponent2D.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <NazaraSDK/World.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Components/PhysicsComponent2D.hpp>
#include <NazaraSDK/Systems/PhysicsSystem2D.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::CollisionComponent2D
	* \brief NDK class that represents a two-dimensional collision geometry
	*/

	/*!
	* \brief Gets the collision box representing the entity
	* \return The physics collision box
	*/
	Nz::Rectf CollisionComponent2D::GetAABB() const
	{
		return GetRigidBody()->GetAABB();
	}

	/*!
	* \brief Gets the position offset between the actual rigid body center of mass position and the origin of the geometry
	* \return Position offset
	*/
	const Nz::Vector2f& CollisionComponent2D::GetGeomOffset() const
	{
		return GetRigidBody()->GetPositionOffset();
	}

	/*!
	* \brief Convenience function to align center of geometry to a specific point
	*
	* \param geomOffset Position offset
	*
	* \remark This does not change the center of mass
	*/
	void CollisionComponent2D::Recenter(const Nz::Vector2f& origin)
	{
		const Nz::RigidBody2D* rigidBody = GetRigidBody();
		SetGeomOffset(origin - rigidBody->GetAABB().GetCenter() + rigidBody->GetPositionOffset());
	}

	/*!
	* \brief Sets geometry for the entity
	*
	* \param geom Geometry used for collisions
	*/
	void CollisionComponent2D::SetGeom(Nz::Collider2DRef geom)
	{
		m_geom = std::move(geom);

		GetRigidBody()->SetGeom(m_geom);
	}

	/*!
	* \brief Sets the position offset between the actual rigid body center of mass position and the origin of the geometry
	*
	* \param geomOffset Position offset
	*/
	void CollisionComponent2D::SetGeomOffset(const Nz::Vector2f& geomOffset)
	{
		GetRigidBody()->SetPositionOffset(geomOffset);
	}

	/*!
	* \brief Initializes the static body
	*
	* \remark Produces a NazaraAssert if entity is invalid
	* \remark Produces a NazaraAssert if entity is not linked to a world, or the world has no physics system
	*/
	void CollisionComponent2D::InitializeStaticBody()
	{
		NazaraAssert(m_entity, "Invalid entity");
		World* entityWorld = m_entity->GetWorld();

		NazaraAssert(entityWorld, "Entity must have world");
		NazaraAssert(entityWorld->HasSystem<PhysicsSystem2D>(), "World must have a physics system");
		Nz::PhysWorld2D& physWorld = entityWorld->GetSystem<PhysicsSystem2D>().GetPhysWorld();

		m_staticBody = std::make_unique<Nz::RigidBody2D>(&physWorld, 0.f, m_geom);
		m_staticBody->SetUserdata(reinterpret_cast<void*>(static_cast<std::ptrdiff_t>(m_entity->GetId())));

		Nz::Matrix4f matrix;
		if (m_entity->HasComponent<NodeComponent>())
			matrix = m_entity->GetComponent<NodeComponent>().GetTransformMatrix();
		else
			matrix.MakeIdentity();

		m_staticBody->SetPosition(Nz::Vector2f(matrix.GetTranslation()));
	}

	Nz::RigidBody2D* CollisionComponent2D::GetRigidBody()
	{
		if (m_entity->HasComponent<PhysicsComponent2D>())
		{
			PhysicsComponent2D& physComponent = m_entity->GetComponent<PhysicsComponent2D>();
			return physComponent.GetRigidBody();
		}
		else
		{
			NazaraAssert(m_staticBody, "An entity without physics component should have a static body");
			return m_staticBody.get();
		}
	}

	const Nz::RigidBody2D* CollisionComponent2D::GetRigidBody() const
	{
		if (m_entity->HasComponent<PhysicsComponent2D>())
		{
			PhysicsComponent2D& physComponent = m_entity->GetComponent<PhysicsComponent2D>();
			return physComponent.GetRigidBody();
		}
		else
		{
			NazaraAssert(m_staticBody, "An entity without physics component should have a static body");
			return m_staticBody.get();
		}
	}

	/*!
	* \brief Operation to perform when component is attached to an entity
	*/

	void CollisionComponent2D::OnAttached()
	{
		if (!m_entity->HasComponent<PhysicsComponent2D>())
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is attached to this component
	*
	* \param component Component being attached
	*/

	void CollisionComponent2D::OnComponentAttached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent2D>(component))
			m_staticBody.reset();
	}

	/*!
	* \brief Operation to perform when component is detached from this component
	*
	* \param component Component being detached
	*/

	void CollisionComponent2D::OnComponentDetached(BaseComponent& component)
	{
		if (IsComponent<PhysicsComponent2D>(component))
			InitializeStaticBody();
	}

	/*!
	* \brief Operation to perform when component is detached from an entity
	*/

	void CollisionComponent2D::OnDetached()
	{
		m_staticBody.reset();
	}

	ComponentIndex CollisionComponent2D::componentIndex;
}
