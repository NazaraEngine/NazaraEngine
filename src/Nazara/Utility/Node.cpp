// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Node.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	Node::~Node()
	{
		OnNodeRelease(this);

		for (Node* child : m_childs)
		{
			// child->SetParent(nullptr); would try to remove itself from the child list while we iterate on it
			child->m_parent = nullptr;
			child->InvalidateNode(Invalidation::InvalidateRecursively);
			child->OnParenting(nullptr);
		}

		SetParent(nullptr);
	}

	NodeType Node::GetNodeType() const
	{
		return NodeType::Default;
	}

	Node& Node::Interpolate(const Node& nodeA, const Node& nodeB, float interpolation, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				nodeA.EnsureDerivedUpdate();
				nodeB.EnsureDerivedUpdate();

				m_position = ToLocalPosition(Vector3f::Lerp(nodeA.m_derivedPosition, nodeB.m_derivedPosition, interpolation));
				m_rotation = ToLocalRotation(Quaternionf::Slerp(nodeA.m_derivedRotation, nodeB.m_derivedRotation, interpolation));
				m_scale = ToLocalScale(Vector3f::Lerp(nodeA.m_derivedScale, nodeB.m_derivedScale, interpolation));
				break;

			case CoordSys::Local:
				m_position = Vector3f::Lerp(nodeA.m_position, nodeB.m_position, interpolation);
				m_rotation = Quaternionf::Slerp(nodeA.m_rotation, nodeB.m_rotation, interpolation);
				m_scale = Vector3f::Lerp(nodeA.m_scale, nodeB.m_scale, interpolation);
				break;
		}

		Invalidate(invalidation);

		return *this;
	}

	Node& Node::Move(const Vector3f& movement, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
			{
				if (m_parent)
				{
					m_parent->EnsureDerivedUpdate();

					m_position += (m_parent->m_derivedRotation.GetConjugate()*(movement - m_parent->m_derivedPosition))/m_parent->m_derivedScale; // Compensation
				}
				else
					m_position += movement; // Rien n'affecte le node

				break;
			}

			case CoordSys::Local:
				m_position += m_rotation * movement;
				break;
		}

		Invalidate(invalidation);

		return *this;
	}

	Node& Node::Rotate(const Quaternionf& rotation, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
			{
				EnsureDerivedUpdate();

				m_rotation *= m_derivedRotation.GetInverse() * rotation * m_derivedRotation; ///FIXME: Correct ?
				break;
			}

			case CoordSys::Local:
				m_rotation *= rotation;
				break;
		}

		m_rotation.Normalize();

		Invalidate(invalidation);

		return *this;
	}

	void Node::SetParent(const Node* node, bool keepDerived, Invalidation invalidation)
	{
		#if NAZARA_UTILITY_SAFE
		// On vérifie que le node n'est pas son propre parent
		const Node* parentNode = node;
		while (parentNode)
		{
			if (parentNode == this)
			{
				NazaraError("a node cannot be it's own parent");
				return;
			}

			parentNode = parentNode->GetParent();
		}
		#endif

		if (m_parent == node)
			return;

		if (keepDerived)
		{
			EnsureDerivedUpdate();

			if (m_parent)
				m_parent->RemoveChild(this);

			m_parent = node;
			if (m_parent)
				m_parent->AddChild(this);

			SetRotation(m_derivedRotation, CoordSys::Global, Invalidation::DontInvalidate);
			SetScale(m_derivedScale, CoordSys::Global, Invalidation::DontInvalidate);
			SetPosition(m_derivedPosition, CoordSys::Global, Invalidation::DontInvalidate);

			Invalidate(invalidation);
		}
		else
		{
			if (m_parent)
				m_parent->RemoveChild(this);

			m_parent = node;
			if (m_parent)
				m_parent->AddChild(this);

			Invalidate(invalidation);
		}

		OnParenting(node);
	}

	void Node::SetPosition(const Vector3f& position, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
			{
				if (m_parent && m_inheritPosition)
				{
					m_parent->EnsureDerivedUpdate();

					m_position = (m_parent->m_derivedRotation.GetConjugate() * (position - m_parent->m_derivedPosition)) / m_parent->m_derivedScale - m_initialPosition;
				}
				else
					m_position = position - m_initialPosition;
				break;
			}

			case CoordSys::Local:
				m_position = position;
				break;
		}

		Invalidate(invalidation);
	}

	void Node::SetRotation(const Quaternionf& rotation, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				if (m_parent && m_inheritRotation)
				{
					Quaternionf rot(m_parent->GetRotation() * m_initialRotation);

					m_rotation = rot.GetConjugate() * rotation;
				}
				else
					m_rotation = rotation;

				break;

			case CoordSys::Local:
				m_rotation = rotation;
				break;
		}

		Invalidate(invalidation);
	}

	void Node::SetScale(const Vector3f& scale, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				if (m_parent && m_inheritScale)
					m_scale = scale / (m_initialScale * m_parent->GetScale());
				else
					m_scale = scale / m_initialScale;
				break;

			case CoordSys::Local:
				m_scale = scale;
				break;
		}

		Invalidate(invalidation);
	}

	void Node::SetTransform(const Vector3f& position, const Quaternionf& rotation, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
			{
				// Position
				if (m_parent && m_inheritPosition)
				{
					m_parent->EnsureDerivedUpdate();

					m_position = (m_parent->m_derivedRotation.GetConjugate() * (position - m_parent->m_derivedPosition)) / m_parent->m_derivedScale - m_initialPosition;
				}
				else
					m_position = position - m_initialPosition;

				// Rotation
				if (m_parent && m_inheritRotation)
				{
					Quaternionf rot(m_parent->GetRotation() * m_initialRotation);

					m_rotation = rot.GetConjugate() * rotation;
				}
				else
					m_rotation = rotation;

				break;
			}

			case CoordSys::Local:
				m_position = position;
				m_rotation = rotation;
				break;
		}

		Invalidate(invalidation);
	}

	void Node::SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale, CoordSys coordSys, Invalidation invalidation)
	{
		switch (coordSys)
		{
			case CoordSys::Global:
			{
				// Position
				if (m_parent && m_inheritPosition)
				{
					m_parent->EnsureDerivedUpdate();

					m_position = (m_parent->m_derivedRotation.GetConjugate() * (position - m_parent->m_derivedPosition)) / m_parent->m_derivedScale - m_initialPosition;
				}
				else
					m_position = position - m_initialPosition;

				// Rotation
				if (m_parent && m_inheritRotation)
				{
					Quaternionf rot(m_parent->GetRotation() * m_initialRotation);

					m_rotation = rot.GetConjugate() * rotation;
				}
				else
					m_rotation = rotation;

				// Scale
				if (m_parent && m_inheritScale)
					m_scale = scale / (m_initialScale * m_parent->GetScale());
				else
					m_scale = scale / m_initialScale;

				break;
			}

			case CoordSys::Local:
				m_position = position;
				m_rotation = rotation;
				m_scale = scale;
				break;
		}

		Invalidate(invalidation);
	}

	void Node::InvalidateNode(Invalidation invalidation)
	{
		m_derivedUpdated = false;
		m_transformMatrixUpdated = false;

		if (invalidation == Invalidation::InvalidateRecursively)
		{
			for (Node* node : m_childs)
				node->InvalidateNode(invalidation);
		}

		OnNodeInvalidation(this);
	}

	void Node::OnParenting(const Node* parent)
	{
		OnNodeNewParent(this, parent);
	}

	void Node::UpdateDerived() const
	{
		if (m_parent)
		{
			m_parent->EnsureDerivedUpdate();

			if (m_inheritPosition)
				m_derivedPosition = m_parent->m_derivedRotation*(m_parent->m_derivedScale * (m_initialPosition + m_position)) + m_parent->m_derivedPosition;
			else
				m_derivedPosition = m_initialPosition + m_position;

			if (m_inheritRotation)
			{
				Quaternionf rotation = m_initialRotation * m_rotation;
				if (m_inheritScale)
					rotation = Quaternionf::Mirror(rotation, m_parent->m_derivedScale);

				m_derivedRotation = m_parent->m_derivedRotation * rotation;
				m_derivedRotation.Normalize();
			}
			else
				m_derivedRotation = m_initialRotation * m_rotation;

			m_derivedScale = m_initialScale * m_scale;
			if (m_inheritScale)
				m_derivedScale *= m_parent->m_derivedScale;
		}
		else
		{
			m_derivedPosition = m_initialPosition + m_position;
			m_derivedRotation = m_initialRotation * m_rotation;
			m_derivedScale = m_initialScale * m_scale;
		}

		m_derivedUpdated = true;
	}

	void Node::UpdateTransformMatrix() const
	{
		EnsureDerivedUpdate();

		m_transformMatrix = Matrix4f::Transform(m_derivedPosition, m_derivedRotation, m_derivedScale);
		m_transformMatrixUpdated = true;
	}
}
