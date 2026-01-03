// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Node.hpp>

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

		if (m_parent)
			m_parent->RemoveChild(this);
	}

	void Node::SetParent(const Node* node, bool keepDerived, Invalidation invalidation)
	{
		#ifdef NAZARA_DEBUG
		// Check the node isn't its own parent
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
			EnsureGlobalsUpdate();

			if (m_parent)
				m_parent->RemoveChild(this);

			m_parent = node;
			if (m_parent)
				m_parent->AddChild(this);

			SetGlobalPosition(m_globalPosition, Invalidation::DontInvalidate);
			SetGlobalRotation(m_globalRotation, Invalidation::DontInvalidate);
			SetGlobalScale(m_globalScale, Invalidation::DontInvalidate);

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
			m_parent->EnsureGlobalsUpdate();

			if (m_doesInheritPosition)
				m_globalPosition = m_parent->m_globalRotation * (m_parent->m_globalScale * m_position) + m_parent->m_globalPosition;
			else
				m_globalPosition = m_position;

			if (m_doesInheritRotation)
			{
				Quaternionf rotation = m_rotation;
				if (m_doesInheritScale)
					rotation = Quaternionf::Mirror(rotation, m_parent->m_globalScale);

				m_globalRotation = m_parent->m_globalRotation * rotation;
				m_globalRotation.Normalize();
			}
			else
				m_globalRotation = m_rotation;

			m_globalScale = m_scale;
			if (m_doesInheritScale)
				m_globalScale *= m_parent->m_globalScale;
		}
		else
		{
			m_globalPosition = m_position;
			m_globalRotation = m_rotation;
			m_globalScale = m_scale;
		}

		m_derivedUpdated = true;
	}

	void Node::UpdateTransformMatrix() const
	{
		EnsureGlobalsUpdate();

		m_transformMatrix = Matrix4f::Transform(m_globalPosition, m_globalRotation, m_globalScale);
		m_transformMatrixUpdated = true;
	}
}
