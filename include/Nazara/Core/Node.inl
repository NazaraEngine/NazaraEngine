// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline Node::Node(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale) :
	m_initialRotation(Quaternionf::Identity()),
	m_rotation(rotation),
	m_initialPosition(Vector3f::Zero()),
	m_initialScale(Vector3f(1.f, 1.f, 1.f)),
	m_position(translation),
	m_scale(scale),
	m_parent(nullptr),
	m_derivedUpdated(false),
	m_doesInheritPosition(true),
	m_doesInheritRotation(true),
	m_doesInheritScale(true),
	m_transformMatrixUpdated(false)
	{
	}

	inline Node::Node(const Node& node) :
	m_initialRotation(node.m_initialRotation),
	m_rotation(node.m_rotation),
	m_initialPosition(node.m_initialPosition),
	m_initialScale(node.m_initialScale),
	m_position(node.m_position),
	m_scale(node.m_scale),
	m_parent(nullptr),
	m_derivedUpdated(false),
	m_doesInheritPosition(node.m_doesInheritPosition),
	m_doesInheritRotation(node.m_doesInheritRotation),
	m_doesInheritScale(node.m_doesInheritScale),
	m_transformMatrixUpdated(false)
	{
		SetParent(node.m_parent, false);
	}

	inline Node::Node(Node&& node) noexcept :
	OnNodeInvalidation(std::move(node.OnNodeInvalidation)),
	OnNodeNewParent(std::move(node.OnNodeNewParent)),
	OnNodeRelease(std::move(node.OnNodeRelease)),
	m_childs(std::move(node.m_childs)),
	m_initialRotation(node.m_initialRotation),
	m_rotation(node.m_rotation),
	m_initialPosition(node.m_initialPosition),
	m_initialScale(node.m_initialScale),
	m_position(node.m_position),
	m_scale(node.m_scale),
	m_parent(node.m_parent),
	m_derivedUpdated(false),
	m_doesInheritPosition(node.m_doesInheritPosition),
	m_doesInheritRotation(node.m_doesInheritRotation),
	m_doesInheritScale(node.m_doesInheritScale),
	m_transformMatrixUpdated(false)
	{
		if (m_parent)
		{
			m_parent->RemoveChild(&node);
			m_parent->AddChild(this);
			node.m_parent = nullptr;
		}

		for (Node* child : m_childs)
			child->m_parent = this;
	}

	inline bool Node::DoesInheritPosition() const
	{
		return m_doesInheritPosition;
	}

	inline bool Node::DoesInheritRotation() const
	{
		return m_doesInheritRotation;
	}

	inline bool Node::DoesInheritScale() const
	{
		return m_doesInheritScale;
	}

	inline void Node::EnsureGlobalsUpdate() const
	{
		if (!m_derivedUpdated)
			UpdateDerived();
	}

	inline void Node::EnsureTransformMatrixUpdate() const
	{
		if (!m_transformMatrixUpdated)
			UpdateTransformMatrix();
	}

	inline Vector3f Node::GetBackward() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation * Vector3f::Backward();
	}

	inline const std::vector<Node*>& Node::GetChilds() const
	{
		return m_childs;
	}

	inline Vector3f Node::GetDown() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation * Vector3f::Down();
	}

	inline Vector3f Node::GetForward() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation * Vector3f::Forward();
	}

	inline const Vector3f& Node::GetGlobalPosition() const
	{
		EnsureGlobalsUpdate();
		return m_globalPosition;
	}

	inline const Quaternionf& Node::GetGlobalRotation() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation;
	}

	inline const Vector3f& Node::GetGlobalScale() const
	{
		EnsureGlobalsUpdate();
		return m_globalScale;
	}

	inline const Vector3f& Node::GetInitialPosition() const
	{
		return m_initialPosition;
	}

	inline const Quaternionf& Node::GetInitialRotation() const
	{
		return m_initialRotation;
	}

	inline const Vector3f& Node::GetInitialScale() const
	{
		return m_initialScale;
	}

	inline Vector3f Node::GetLeft() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation * Vector3f::Left();
	}

	inline const Node* Node::GetParent() const
	{
		return m_parent;
	}

	inline const Vector3f& Node::GetPosition() const
	{
		return m_position;
	}

	inline Vector3f Node::GetRight() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation * Vector3f::Right();
	}

	inline const Quaternionf& Node::GetRotation() const
	{
		return m_rotation;
	}

	inline const Vector3f& Node::GetScale() const
	{
		return m_scale;
	}

	inline const Matrix4f& Node::GetTransformMatrix() const
	{
		EnsureTransformMatrixUpdate();
		return m_transformMatrix;
	}

	inline Vector3f Node::GetUp() const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation * Vector3f::Up();
	}

	inline bool Node::HasChilds() const
	{
		return !m_childs.empty();
	}

	inline void Node::Invalidate(Invalidation invalidation)
	{
		if (invalidation != Invalidation::DontInvalidate)
			InvalidateNode(invalidation);
	}

	inline Node& Node::Interpolate(const Node& nodeA, const Node& nodeB, float interpolation, Invalidation invalidation)
	{
		m_position = Vector3f::Lerp(nodeA.m_position, nodeB.m_position, interpolation);
		m_rotation = Quaternionf::Slerp(nodeA.m_rotation, nodeB.m_rotation, interpolation);
		m_scale = Vector3f::Lerp(nodeA.m_scale, nodeB.m_scale, interpolation);

		Invalidate(invalidation);
		return *this;
	}
	
	Node& Node::InterpolateGlobal(const Node& nodeA, const Node& nodeB, float interpolation, Invalidation invalidation)
	{
		nodeA.EnsureGlobalsUpdate();
		nodeB.EnsureGlobalsUpdate();

		m_position = ToLocalPosition(Vector3f::Lerp(nodeA.m_globalPosition, nodeB.m_globalPosition, interpolation));
		m_rotation = ToLocalRotation(Quaternionf::Slerp(nodeA.m_globalRotation, nodeB.m_globalRotation, interpolation));
		m_scale = ToLocalScale(Vector3f::Lerp(nodeA.m_globalScale, nodeB.m_globalScale, interpolation));

		Invalidate(invalidation);
		return *this;
	}

	inline Node& Node::Move(const Vector2f& movement, Invalidation invalidation)
	{
		return Move(Vector3f(movement), invalidation);
	}

	inline Node& Node::Move(const Vector3f& movement, Invalidation invalidation)
	{
		m_position += m_rotation * movement;

		Invalidate(invalidation);
		return *this;
	}

	inline Node& Node::MoveGlobal(const Vector2f& movement, Invalidation invalidation)
	{
		return MoveGlobal(Vector3f(movement), invalidation);
	}

	inline Node& Node::MoveGlobal(const Vector3f& movement, Invalidation invalidation)
	{
		if (m_parent)
		{
			m_parent->EnsureGlobalsUpdate();

			m_position += (m_parent->m_globalRotation.GetConjugate() * (movement - m_parent->m_globalPosition)) / m_parent->m_globalScale;
		}
		else
			m_position += movement;

		Invalidate(invalidation);
		return *this;
	}
	
	inline Node& Node::Rotate(const Quaternionf& rotation, Invalidation invalidation)
	{
		m_rotation = rotation * m_rotation;
		m_rotation.Normalize();

		Invalidate(invalidation);
		return *this;
	}

	inline Node& Node::RotateGlobal(const Quaternionf& rotation, Invalidation invalidation)
	{
		EnsureGlobalsUpdate();

		m_rotation = m_rotation * m_globalRotation.GetConjugate() * rotation * m_globalRotation;
		m_rotation.Normalize();

		Invalidate(invalidation);
		return *this;
	}

	inline Node& Node::Scale(const Vector3f& scale, Invalidation invalidation)
	{
		m_scale *= scale;

		Invalidate(invalidation);
		return *this;
	}

	inline Node& Node::Scale(float scale, Invalidation invalidation)
	{
		m_scale *= scale;

		Invalidate(invalidation);
		return *this;
	}

	inline Node& Node::Scale(const Vector2f& scale, Invalidation invalidation)
	{
		return Scale(Vector3f(scale), invalidation);
	}

	inline void Node::SetGlobalPosition(const Vector2f& position, Invalidation invalidation)
	{
		return SetGlobalPosition(Vector3f(position), invalidation);
	}

	inline void Node::SetGlobalPosition(const Vector3f& position, Invalidation invalidation)
	{
		if (m_parent && m_doesInheritPosition)
		{
			m_parent->EnsureGlobalsUpdate();

			m_position = (m_parent->m_globalRotation.GetConjugate() * (position - m_parent->m_globalPosition)) / m_parent->m_globalScale - m_initialPosition;
		}
		else
			m_position = position - m_initialPosition;

		Invalidate(invalidation);
	}

	inline void Node::SetGlobalRotation(const Quaternionf& rotation, Invalidation invalidation)
	{
		if (m_parent && m_doesInheritRotation)
		{
			Quaternionf rot(m_parent->GetRotation() * m_initialRotation);

			m_rotation = rot.GetConjugate() * rotation;
		}
		else
			m_rotation = rotation;

		Invalidate(invalidation);
	}

	inline void Node::SetGlobalScale(const Vector2f& scale, Invalidation invalidation)
	{
		return SetGlobalScale(Vector3f(scale.x, scale.y, 1.f), invalidation);
	}

	inline void Node::SetGlobalScale(const Vector3f& scale, Invalidation invalidation)
	{
		if (m_parent && m_doesInheritScale)
			m_scale = scale / (m_initialScale * m_parent->m_globalScale);
		else
			m_scale = scale / m_initialScale;

		Invalidate(invalidation);
	}
	
	inline void Node::SetGlobalTransform(const Vector3f& position, const Quaternionf& rotation, Invalidation invalidation)
	{
		// Position
		if (m_parent && m_doesInheritPosition)
		{
			m_parent->EnsureGlobalsUpdate();

			m_position = (m_parent->m_globalRotation.GetConjugate() * (position - m_parent->m_globalPosition)) / m_parent->m_globalScale - m_initialPosition;
		}
		else
			m_position = position - m_initialPosition;

		// Rotation
		if (m_parent && m_doesInheritRotation)
		{
			Quaternionf rot(m_parent->GetRotation() * m_initialRotation);

			m_rotation = rot.GetConjugate() * rotation;
		}
		else
			m_rotation = rotation;

		Invalidate(invalidation);
	}

	inline void Node::SetGlobalTransform(const Vector3f& position, const Quaternionf& rotation, const Vector2f& scale, Invalidation invalidation)
	{
		return SetGlobalTransform(position, rotation, Vector3f(scale.x, scale.y, 1.f), invalidation);
	}

	inline void Node::SetGlobalTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale, Invalidation invalidation)
	{
		// Position
		if (m_parent && m_doesInheritPosition)
		{
			m_parent->EnsureGlobalsUpdate();

			m_position = (m_parent->m_globalRotation.GetConjugate() * (position - m_parent->m_globalPosition)) / m_parent->m_globalScale - m_initialPosition;
		}
		else
			m_position = position - m_initialPosition;

		// Rotation
		if (m_parent && m_doesInheritRotation)
		{
			Quaternionf rot(m_parent->GetRotation() * m_initialRotation);

			m_rotation = rot.GetConjugate() * rotation;
		}
		else
			m_rotation = rotation;

		// Scale
		if (m_parent && m_doesInheritScale)
			m_scale = scale / (m_initialScale * m_parent->GetScale());
		else
			m_scale = scale / m_initialScale;

		Invalidate(invalidation);
	}

	inline void Node::SetInheritPosition(bool inheritPosition, Invalidation invalidation)
	{
		///DOC: Un appel redondant est sans effet
		if (m_doesInheritPosition != inheritPosition)
		{
			m_doesInheritPosition = inheritPosition;

			Invalidate(invalidation);
		}
	}

	inline void Node::SetInheritRotation(bool inheritRotation, Invalidation invalidation)
	{
		///DOC: Un appel redondant est sans effet
		if (m_doesInheritRotation != inheritRotation)
		{
			m_doesInheritRotation = inheritRotation;

			Invalidate(invalidation);
		}
	}

	inline void Node::SetInheritScale(bool inheritScale, Invalidation invalidation)
	{
		///DOC: Un appel redondant est sans effet
		if (m_doesInheritScale != inheritScale)
		{
			m_doesInheritScale = inheritScale;

			Invalidate(invalidation);
		}
	}

	inline void Node::SetInitialPosition(const Vector3f& position, Invalidation invalidation)
	{
		m_initialPosition = position;

		Invalidate(invalidation);
	}

	inline void Node::SetInitialPosition(float positionX, float positionY, float positionZ, Invalidation invalidation)
	{
		m_initialPosition = Vector3f(positionX, positionY, positionZ);

		Invalidate(invalidation);
	}

	inline void Node::SetInitialRotation(const Quaternionf& rotation, Invalidation invalidation)
	{
		m_initialRotation = rotation;

		Invalidate(invalidation);
	}

	inline void Node::SetInitialScale(const Vector3f& scale, Invalidation invalidation)
	{
		m_initialScale = scale;

		Invalidate(invalidation);
	}

	inline void Node::SetInitialScale(float scale, Invalidation invalidation)
	{
		m_initialScale = Vector3f(scale);

		Invalidate(invalidation);
	}

	inline void Node::SetInitialScale(float scaleX, float scaleY, float scaleZ, Invalidation invalidation)
	{
		m_initialScale = Vector3f(scaleX, scaleY, scaleZ);

		Invalidate(invalidation);
	}

	inline void Node::SetParent(const Node& node, bool keepDerived, Invalidation invalidation)
	{
		SetParent(&node, keepDerived, invalidation);
	}

	inline void Node::SetPosition(const Vector2f& position, Invalidation invalidation)
	{
		m_position = Vector3f(position);
		Invalidate(invalidation);
	}
	
	inline void Node::SetPosition(const Vector3f& position, Invalidation invalidation)
	{
		m_position = position;
		Invalidate(invalidation);
	}

	inline void Node::SetRotation(const Quaternionf& rotation, Invalidation invalidation)
	{
		m_rotation = rotation;
		Invalidate(invalidation);
	}

	inline void Node::SetScale(float scale, Invalidation invalidation)
	{
		return SetScale(Vector3f(scale, scale, scale), invalidation);
	}

	inline void Node::SetScale(const Vector2f& scale, Invalidation invalidation)
	{
		return SetScale(Vector3f(scale.x, scale.y, 1.f), invalidation);
	}

	inline void Node::SetScale(const Vector3f& scale, Invalidation invalidation)
	{
		m_scale = scale;
		Invalidate(invalidation);
	}
	
	inline void Node::SetTransform(const Vector3f& position, const Quaternionf& rotation, Invalidation invalidation)
	{
		m_position = position;
		m_rotation = rotation;

		Invalidate(invalidation);
	}

	inline void Node::SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector2f& scale, Invalidation invalidation)
	{
		return SetTransform(position, rotation, Vector3f(scale.x, scale.y, 1.f), invalidation);
	}

	inline void Node::SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale, Invalidation invalidation)
	{
		m_position = position;
		m_rotation = rotation;
		m_scale = scale;

		Invalidate(invalidation);
	}

	inline void Node::SetTransformMatrix(const Matrix4f& matrix, Invalidation invalidation)
	{
		SetGlobalPosition(matrix.GetTranslation(), Invalidation::DontInvalidate);
		SetGlobalRotation(matrix.GetRotation(), Invalidation::DontInvalidate);
		SetGlobalScale(matrix.GetScale(), Invalidation::DontInvalidate);

		Invalidate(invalidation);

		m_transformMatrix = matrix;
		m_transformMatrixUpdated = true;
	}

	inline Vector3f Node::ToGlobalPosition(const Vector3f& localPosition) const
	{
		EnsureGlobalsUpdate();
		return TransformPositionTRS(m_globalPosition, m_globalRotation, m_globalScale, localPosition);
	}

	inline Quaternionf Node::ToGlobalRotation(const Quaternionf& localRotation) const
	{
		EnsureGlobalsUpdate();
		return TransformRotationTRS(m_globalRotation, m_globalScale, localRotation);
	}

	inline Vector3f Node::ToGlobalScale(const Vector3f& localScale) const
	{
		EnsureGlobalsUpdate();
		return TransformScaleTRS(m_globalScale, localScale);
	}

	inline Vector3f Node::ToLocalPosition(const Vector3f& globalPosition) const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation.GetConjugate() * (globalPosition - m_globalPosition) / m_globalScale;
	}

	inline Quaternionf Node::ToLocalRotation(const Quaternionf& globalRotation) const
	{
		EnsureGlobalsUpdate();
		return m_globalRotation.GetConjugate() * globalRotation;
	}

	inline Vector3f Node::ToLocalScale(const Vector3f& globalScale) const
	{
		EnsureGlobalsUpdate();
		return globalScale / m_globalScale;
	}

	inline Node& Node::operator=(const Node& node)
	{
		SetParent(node.m_parent, false, Invalidation::DontInvalidate);

		m_doesInheritPosition = node.m_doesInheritPosition;
		m_doesInheritRotation = node.m_doesInheritRotation;
		m_doesInheritScale = node.m_doesInheritScale;
		m_initialPosition = node.m_initialPosition;
		m_initialRotation = node.m_initialRotation;
		m_initialScale = node.m_initialScale;
		m_position = node.m_position;
		m_rotation = node.m_rotation;
		m_scale = node.m_scale;

		InvalidateNode(Invalidation::InvalidateRecursively);

		return *this;
	}

	inline Node& Node::operator=(Node&& node) noexcept
	{
		if (m_parent)
			SetParent(nullptr);

		m_doesInheritPosition = node.m_doesInheritPosition;
		m_doesInheritRotation = node.m_doesInheritRotation;
		m_doesInheritScale = node.m_doesInheritScale;
		m_initialPosition = node.m_initialPosition;
		m_initialRotation = node.m_initialRotation;
		m_initialScale = node.m_initialScale;
		m_position = node.m_position;
		m_rotation = node.m_rotation;
		m_scale = node.m_scale;

		m_childs = std::move(node.m_childs);
		for (Node* child : m_childs)
			child->m_parent = this;

		m_parent = node.m_parent;
		if (m_parent)
		{
			m_parent->RemoveChild(&node);
			m_parent->AddChild(this);
			node.m_parent = nullptr;
		}

		OnNodeInvalidation = std::move(node.OnNodeInvalidation);
		OnNodeNewParent = std::move(node.OnNodeNewParent);
		OnNodeRelease = std::move(node.OnNodeRelease);

		InvalidateNode(Invalidation::InvalidateRecursively);

		return *this;
	}

	inline void Node::AddChild(Node* node) const
	{
#ifdef NAZARA_DEBUG
		if (std::find(m_childs.begin(), m_childs.end(), node) != m_childs.end())
		{
			NazaraWarning("Child node is already a child of parent node");
			return;
		}
#endif

		m_childs.push_back(node);
	}

	inline void Node::RemoveChild(Node* node) const
	{
		auto it = std::find(m_childs.begin(), m_childs.end(), node);
		if (it != m_childs.end())
			m_childs.erase(it);
		else
			NazaraWarning("Child not found");
	}
}

#include <Nazara/Core/DebugOff.hpp>
