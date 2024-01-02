// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Utility/Algorithm.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

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
	m_inheritPosition(true),
	m_inheritRotation(true),
	m_inheritScale(true),
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
	m_inheritPosition(node.m_inheritPosition),
	m_inheritRotation(node.m_inheritRotation),
	m_inheritScale(node.m_inheritScale),
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
	m_inheritPosition(node.m_inheritPosition),
	m_inheritRotation(node.m_inheritRotation),
	m_inheritScale(node.m_inheritScale),
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

	inline void Node::EnsureDerivedUpdate() const
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
		EnsureDerivedUpdate();
		return m_derivedRotation * Vector3f::Backward();
	}

	inline const std::vector<Node*>& Node::GetChilds() const
	{
		return m_childs;
	}

	inline Vector3f Node::GetDown() const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation * Vector3f::Down();
	}

	inline Vector3f Node::GetForward() const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation * Vector3f::Forward();
	}

	inline bool Node::GetInheritPosition() const
	{
		return m_inheritPosition;
	}

	inline bool Node::GetInheritRotation() const
	{
		return m_inheritRotation;
	}

	inline bool Node::GetInheritScale() const
	{
		return m_inheritScale;
	}

	inline Vector3f Node::GetInitialPosition() const
	{
		return m_initialPosition;
	}

	inline Quaternionf Node::GetInitialRotation() const
	{
		return m_initialRotation;
	}

	inline Vector3f Node::GetInitialScale() const
	{
		return m_initialScale;
	}

	inline Vector3f Node::GetLeft() const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation * Vector3f::Left();
	}

	inline const Node* Node::GetParent() const
	{
		return m_parent;
	}

	inline Vector3f Node::GetPosition(CoordSys coordSys) const
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				EnsureDerivedUpdate();
				return m_derivedPosition;

			case CoordSys::Local:
				return m_position;
		}

		NazaraErrorFmt("Coordinate system out of enum ({0:#x})", UnderlyingCast(coordSys));
		return Vector3f();
	}

	inline Vector3f Node::GetRight() const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation * Vector3f::Right();
	}

	inline Quaternionf Node::GetRotation(CoordSys coordSys) const
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				EnsureDerivedUpdate();
				return m_derivedRotation;

			case CoordSys::Local:
				return m_rotation;
		}

		NazaraErrorFmt("Coordinate system out of enum ({0:#x})", UnderlyingCast(coordSys));
		return Quaternionf();
	}

	inline Vector3f Node::GetScale(CoordSys coordSys) const
	{
		switch (coordSys)
		{
			case CoordSys::Global:
				EnsureDerivedUpdate();
				return m_derivedScale;

			case CoordSys::Local:
				return m_scale;
		}

		NazaraErrorFmt("Coordinate system out of enum ({0:#x})", UnderlyingCast(coordSys));
		return Vector3f();
	}

	inline const Matrix4f& Node::GetTransformMatrix() const
	{
		EnsureTransformMatrixUpdate();
		return m_transformMatrix;
	}

	inline Vector3f Node::GetUp() const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation * Vector3f::Up();
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

	inline Node& Node::Move(float moveX, float moveY, float moveZ, CoordSys coordSys, Invalidation invalidation)
	{
		return Move(Vector3f(moveX, moveY, moveZ), coordSys, invalidation);
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

	inline Node& Node::Scale(float scaleX, float scaleY, float scaleZ, Invalidation invalidation)
	{
		m_scale.x *= scaleX;
		m_scale.y *= scaleY;
		m_scale.z *= scaleZ;

		Invalidate(invalidation);

		return *this;
	}

	inline void Node::SetInheritPosition(bool inheritPosition, Invalidation invalidation)
	{
		///DOC: Un appel redondant est sans effet
		if (m_inheritPosition != inheritPosition)
		{
			m_inheritPosition = inheritPosition;

			Invalidate(invalidation);
		}
	}

	inline void Node::SetInheritRotation(bool inheritRotation, Invalidation invalidation)
	{
		///DOC: Un appel redondant est sans effet
		if (m_inheritRotation != inheritRotation)
		{
			m_inheritRotation = inheritRotation;

			Invalidate(invalidation);
		}
	}

	inline void Node::SetInheritScale(bool inheritScale, Invalidation invalidation)
	{
		///DOC: Un appel redondant est sans effet
		if (m_inheritScale != inheritScale)
		{
			m_inheritScale = inheritScale;

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

	inline void Node::SetPosition(float positionX, float positionY, float positionZ, CoordSys coordSys, Invalidation invalidation)
	{
		SetPosition(Vector3f(positionX, positionY, positionZ), coordSys, invalidation);
	}

	inline void Node::SetScale(const Vector2f& scale, CoordSys coordSys, Invalidation invalidation)
	{
		// Prevent Z scale at zero (can happen when using SetScale with a Vec2)
		SetScale(scale.x, scale.y, 1.f, coordSys, invalidation);
	}

	inline void Node::SetScale(float scale, CoordSys coordSys, Invalidation invalidation)
	{
		SetScale(Vector3f(scale), coordSys, invalidation);
	}

	inline void Node::SetScale(float scaleX, float scaleY, float scaleZ, CoordSys coordSys, Invalidation invalidation)
	{
		SetScale(Vector3f(scaleX, scaleY, scaleZ), coordSys, invalidation);
	}

	inline void Node::SetTransformMatrix(const Matrix4f& matrix, Invalidation invalidation)
	{
		SetPosition(matrix.GetTranslation(), CoordSys::Global, Invalidation::DontInvalidate);
		SetRotation(matrix.GetRotation(), CoordSys::Global, Invalidation::DontInvalidate);
		SetScale(matrix.GetScale(), CoordSys::Global, Invalidation::DontInvalidate);

		Invalidate(invalidation);

		m_transformMatrix = matrix;
		m_transformMatrixUpdated = true;
	}

	inline Vector3f Node::ToGlobalPosition(const Vector3f& localPosition) const
	{
		EnsureDerivedUpdate();
		return TransformPositionTRS(m_derivedPosition, m_derivedRotation, m_derivedScale, localPosition);
	}

	inline Quaternionf Node::ToGlobalRotation(const Quaternionf& localRotation) const
	{
		EnsureDerivedUpdate();
		return TransformRotationTRS(m_derivedRotation, m_derivedScale, localRotation);
	}

	inline Vector3f Node::ToGlobalScale(const Vector3f& localScale) const
	{
		EnsureDerivedUpdate();
		return TransformScaleTRS(m_derivedScale, localScale);
	}

	inline Vector3f Node::ToLocalPosition(const Vector3f& globalPosition) const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation.GetConjugate() * (globalPosition - m_derivedPosition) / m_derivedScale;
	}

	inline Quaternionf Node::ToLocalRotation(const Quaternionf& globalRotation) const
	{
		EnsureDerivedUpdate();
		return m_derivedRotation.GetConjugate() * globalRotation;
	}

	inline Vector3f Node::ToLocalScale(const Vector3f& globalScale) const
	{
		EnsureDerivedUpdate();
		return globalScale / m_derivedScale;
	}
	
	inline Node& Node::operator=(const Node& node)
	{
		SetParent(node.m_parent, false, Invalidation::DontInvalidate);

		m_inheritPosition = node.m_inheritPosition;
		m_inheritRotation = node.m_inheritRotation;
		m_inheritScale = node.m_inheritScale;
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

		m_inheritPosition = node.m_inheritPosition;
		m_inheritRotation = node.m_inheritRotation;
		m_inheritScale = node.m_inheritScale;
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

#include <Nazara/Utility/DebugOff.hpp>
