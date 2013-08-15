// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Node.hpp>
#include <Nazara/Utility/Debug.hpp>

NzNode::NzNode() :
m_initialRotation(NzQuaternionf::Identity()),
m_rotation(NzQuaternionf::Identity()),
m_initialPosition(NzVector3f::Zero()),
m_initialScale(NzVector3f(1.f, 1.f, 1.f)),
m_position(NzVector3f::Zero()),
m_scale(NzVector3f(1.f, 1.f, 1.f)),
m_parent(nullptr),
m_derivedUpdated(false),
m_inheritPosition(true),
m_inheritRotation(true),
m_inheritScale(true),
m_transformMatrixUpdated(false)
{
}

NzNode::NzNode(const NzNode& node) :
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
		m_parent->AddChild(this);
}

NzNode::~NzNode()
{
	for (NzNode* child : m_childs)
		child->SetParent(nullptr);

	SetParent(nullptr);
}

void NzNode::EnsureDerivedUpdate() const
{
	if (!m_derivedUpdated)
		UpdateDerived();
}

void NzNode::EnsureTransformMatrixUpdate() const
{
	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();
}

NzVector3f NzNode::GetBackward() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * NzVector3f::Backward();
}

const std::vector<NzNode*>& NzNode::GetChilds() const
{
	return m_childs;
}

NzVector3f NzNode::GetDown() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * NzVector3f::Down();
}

NzVector3f NzNode::GetForward() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * NzVector3f::Forward();
}

bool NzNode::GetInheritPosition() const
{
	return m_inheritPosition;
}

bool NzNode::GetInheritRotation() const
{
	return m_inheritRotation;
}

bool NzNode::GetInheritScale() const
{
	return m_inheritScale;
}

NzVector3f NzNode::GetInitialPosition() const
{
	return m_initialPosition;
}

NzQuaternionf NzNode::GetInitialRotation() const
{
	return m_initialRotation;
}

NzVector3f NzNode::GetInitialScale() const
{
	return m_initialScale;
}

NzVector3f NzNode::GetLeft() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * NzVector3f::Left();
}

const NzString& NzNode::GetName() const
{
	return m_name;
}

nzNodeType NzNode::GetNodeType() const
{
	return nzNodeType_Default;
}

const NzNode* NzNode::GetParent() const
{
	return m_parent;
}

NzVector3f NzNode::GetPosition(nzCoordSys coordSys) const
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (!m_derivedUpdated)
				UpdateDerived();

			return m_derivedPosition;

		case nzCoordSys_Local:
			return m_position;
	}

	NazaraError("Coordinate system out of enum (0x" + NzString::Number(coordSys, 16) + ')');
	return NzVector3f();
}

NzVector3f NzNode::GetRight() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * NzVector3f::Right();
}

NzQuaternionf NzNode::GetRotation(nzCoordSys coordSys) const
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (!m_derivedUpdated)
				UpdateDerived();

			return m_derivedRotation;

		case nzCoordSys_Local:
			return m_rotation;
	}

	NazaraError("Coordinate system out of enum (0x" + NzString::Number(coordSys, 16) + ')');
	return NzQuaternionf();
}

NzVector3f NzNode::GetScale(nzCoordSys coordSys) const
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (!m_derivedUpdated)
				UpdateDerived();

			return m_derivedScale;

		case nzCoordSys_Local:
			return m_scale;
	}

	NazaraError("Coordinate system out of enum (0x" + NzString::Number(coordSys, 16) + ')');
	return NzVector3f();
}

const NzMatrix4f& NzNode::GetTransformMatrix() const
{
	if (!m_transformMatrixUpdated)
		UpdateTransformMatrix();

	return m_transformMatrix;
}

NzVector3f NzNode::GetUp() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * NzVector3f::Up();
}

bool NzNode::HasChilds() const
{
	return !m_childs.empty();
}

NzNode& NzNode::Interpolate(const NzNode& nodeA, const NzNode& nodeB, float interpolation, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (!nodeA.m_derivedUpdated)
				nodeA.UpdateDerived();

			if (!nodeB.m_derivedUpdated)
				nodeB.UpdateDerived();

			m_position = ToLocalPosition(NzVector3f::Lerp(nodeA.m_derivedPosition, nodeB.m_derivedPosition, interpolation));
			m_rotation = ToLocalRotation(NzQuaternionf::Slerp(nodeA.m_derivedRotation, nodeB.m_derivedRotation, interpolation));
			m_scale = ToLocalScale(NzVector3f::Lerp(nodeA.m_derivedScale, nodeB.m_derivedScale, interpolation));
			break;

		case nzCoordSys_Local:
			m_position = NzVector3f::Lerp(nodeA.m_position, nodeB.m_position, interpolation);
			m_rotation = NzQuaternionf::Slerp(nodeA.m_rotation, nodeB.m_rotation, interpolation);
			m_scale = NzVector3f::Lerp(nodeA.m_scale, nodeB.m_scale, interpolation);
			break;
	}

	Invalidate();
	return *this;
}

NzNode& NzNode::Move(const NzVector3f& movement, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
		{
			if (m_parent)
			{
				if (!m_parent->m_derivedUpdated)
					m_parent->UpdateDerived();

				m_position += (m_parent->m_derivedRotation.GetConjugate()*(movement - m_parent->m_derivedPosition))/m_parent->m_derivedScale; // Compensation
			}
			else
				m_position += movement; // Rien n'affecte le node

			break;
		}

		case nzCoordSys_Local:
			m_position += m_scale * (m_rotation * movement);
			break;
	}

	Invalidate();
	return *this;
}

NzNode& NzNode::Move(float moveX, float moveY, float moveZ, nzCoordSys coordSys)
{
	return Move(NzVector3f(moveX, moveY, moveZ), coordSys);
}

NzNode& NzNode::Rotate(const NzQuaternionf& rotation, nzCoordSys coordSys)
{
	// Évitons toute mauvaise surprise ..
	NzQuaternionf q(rotation);
	q.Normalize();

	switch (coordSys)
	{
		case nzCoordSys_Global:
		{
			if (!m_derivedUpdated)
				UpdateDerived();

			m_rotation *= m_derivedRotation.GetInverse() * q * m_derivedRotation; ///FIXME: Correct ?
			break;
		}

		case nzCoordSys_Local:
			m_rotation *= q;
			break;
	}

	m_rotation.Normalize();

	Invalidate();
	return *this;
}

NzNode& NzNode::Scale(const NzVector3f& scale)
{
	m_scale *= scale;

	Invalidate();
	return *this;
}

NzNode& NzNode::Scale(float scale)
{
	m_scale *= scale;

	Invalidate();
	return *this;
}

NzNode& NzNode::Scale(float scaleX, float scaleY, float scaleZ)
{
	m_scale.x *= scaleX;
	m_scale.y *= scaleY;
	m_scale.z *= scaleZ;

	Invalidate();
	return *this;
}

void NzNode::SetInheritPosition(bool inheritPosition)
{
	///DOC: Un appel redondant est sans effet
	if (m_inheritPosition != inheritPosition)
	{
		m_inheritPosition = inheritPosition;

		Invalidate();
	}
}

void NzNode::SetInheritRotation(bool inheritRotation)
{
	///DOC: Un appel redondant est sans effet
	if (m_inheritRotation != inheritRotation)
	{
		m_inheritRotation = inheritRotation;

		Invalidate();
	}
}

void NzNode::SetInheritScale(bool inheritScale)
{
	///DOC: Un appel redondant est sans effet
	if (m_inheritScale != inheritScale)
	{
		m_inheritScale = inheritScale;

		Invalidate();
	}
}

void NzNode::SetInitialPosition(const NzVector3f& position)
{
	m_initialPosition = position;

	Invalidate();
}

void NzNode::SetInitialPosition(float positionX, float positionY, float positionZ)
{
	m_initialPosition.Set(positionX, positionY, positionZ);

	Invalidate();
}

void NzNode::SetInitialRotation(const NzQuaternionf& rotation)
{
	m_initialRotation = rotation;
	m_initialRotation.Normalize(); // Évitons toute mauvaise surprise ...

	Invalidate();
}

void NzNode::SetInitialScale(const NzVector3f& scale)
{
	m_initialScale = scale;

	Invalidate();
}

void NzNode::SetInitialScale(float scale)
{
	m_initialScale.Set(scale);

	Invalidate();
}

void NzNode::SetInitialScale(float scaleX, float scaleY, float scaleZ)
{
	m_initialScale.Set(scaleX, scaleY, scaleZ);

	Invalidate();
}

void NzNode::SetName(const NzString& name)
{
	m_name = name;
}

void NzNode::SetParent(const NzNode* node, bool keepDerived)
{
	#if NAZARA_UTILITY_SAFE
	if (node == this)
	{
		NazaraError("A node cannot be it's own parent");
		return;
	}
	#endif

	if (m_parent == node)
		return;

	if (keepDerived)
	{
		if (!m_derivedUpdated)
			UpdateDerived();

		if (m_parent)
			m_parent->RemoveChild(this);

		m_parent = node;
		if (m_parent)
			m_parent->AddChild(this);

		SetRotation(m_derivedRotation, nzCoordSys_Global);
		SetScale(m_derivedScale, nzCoordSys_Global);
		SetPosition(m_derivedPosition, nzCoordSys_Global);
	}
	else
	{
		if (m_parent)
			m_parent->RemoveChild(this);

		m_parent = node;
		if (m_parent)
			m_parent->AddChild(this);

		Invalidate();
	}

	OnParenting(node);
}

void NzNode::SetParent(const NzNode& node, bool keepDerived)
{
	SetParent(&node, keepDerived);
}

void NzNode::SetPosition(const NzVector3f& position, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (m_parent && m_inheritPosition)
			{
				if (!m_parent->m_derivedUpdated)
					m_parent->UpdateDerived();

				m_position = (m_parent->m_derivedRotation.GetConjugate()*(position - m_parent->m_derivedPosition))/m_parent->m_derivedScale - m_initialPosition;
			}
			else
				m_position = position - m_initialPosition;
			break;

		case nzCoordSys_Local:
			m_position = position;
			break;
	}

	Invalidate();
}

void NzNode::SetPosition(float positionX, float positionY, float positionZ, nzCoordSys coordSys)
{
	SetPosition(NzVector3f(positionX, positionY, positionZ), coordSys);
}

void NzNode::SetRotation(const NzQuaternionf& rotation, nzCoordSys coordSys)
{
	// Évitons toute mauvaise surprise ..
	NzQuaternionf q(rotation);
	q.Normalize();

	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (m_parent && m_inheritRotation)
			{
				NzQuaternionf rot(m_parent->GetRotation() * m_initialRotation);

				m_rotation = rot.GetConjugate() * q;
			}
			else
				m_rotation = q;

			break;

		case nzCoordSys_Local:
			m_rotation = q;
			break;
	}

	Invalidate();
}

void NzNode::SetScale(const NzVector3f& scale, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
			if (m_parent && m_inheritScale)
				m_scale = scale / (m_initialScale * m_parent->GetScale());
			else
				m_scale = scale / m_initialScale;
			break;

		case nzCoordSys_Local:
			m_scale = scale;
			break;
	}

	Invalidate();
}

void NzNode::SetScale(float scale, nzCoordSys coordSys)
{
	SetScale(NzVector3f(scale), coordSys);
}

void NzNode::SetScale(float scaleX, float scaleY, float scaleZ, nzCoordSys coordSys)
{
	SetScale(NzVector3f(scaleX, scaleY, scaleZ), coordSys);
}

void NzNode::SetTransformMatrix(const NzMatrix4f& matrix)
{
	SetPosition(matrix.GetTranslation(), nzCoordSys_Global);
	SetRotation(matrix.GetRotation(), nzCoordSys_Global);
	SetScale(matrix.GetScale(), nzCoordSys_Global);

	m_transformMatrix = matrix;
	m_transformMatrixUpdated = true;
}

NzVector3f NzNode::ToGlobalPosition(const NzVector3f& localPosition) const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedPosition + (m_derivedScale * (m_derivedRotation * localPosition));
}

NzQuaternionf NzNode::ToGlobalRotation(const NzQuaternionf& localRotation) const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation * localRotation;
}

NzVector3f NzNode::ToGlobalScale(const NzVector3f& localScale) const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedScale * localScale;
}

NzVector3f NzNode::ToLocalPosition(const NzVector3f& globalPosition) const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return (m_derivedRotation.GetConjugate()*(globalPosition - m_derivedPosition))/m_derivedScale;
}

NzQuaternionf NzNode::ToLocalRotation(const NzQuaternionf& globalRotation) const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation.GetConjugate() * globalRotation;
}

NzVector3f NzNode::ToLocalScale(const NzVector3f& globalScale) const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return globalScale / m_derivedScale;
}

NzNode& NzNode::operator=(const NzNode& node)
{
	SetParent(node.m_parent);

	m_inheritPosition = node.m_inheritPosition;
	m_inheritRotation = node.m_inheritRotation;
	m_inheritScale = node.m_inheritScale;
	m_initialPosition = node.m_initialPosition;
	m_initialRotation = node.m_initialRotation;
	m_initialScale = node.m_initialScale;
	m_name = node.m_name;
	m_position = node.m_position;
	m_rotation = node.m_rotation;
	m_scale = node.m_scale;

	Invalidate();

	return *this;
}

void NzNode::AddChild(NzNode* node) const
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

void NzNode::Invalidate()
{
	m_derivedUpdated = false;
	m_transformMatrixUpdated = false;

	for (NzNode* node : m_childs)
		node->Invalidate();
}

void NzNode::OnParenting(const NzNode* parent)
{
	NazaraUnused(parent);
}

void NzNode::RemoveChild(NzNode* node) const
{
	auto it = std::find(m_childs.begin(), m_childs.end(), node);
	if (it != m_childs.end())
		m_childs.erase(it);
	else
		NazaraWarning("Child not found");
}

void NzNode::UpdateDerived() const
{
	if (m_parent)
	{
		if (!m_parent->m_derivedUpdated)
			m_parent->UpdateDerived();

		if (m_inheritPosition)
			m_derivedPosition = m_parent->m_derivedRotation*(m_parent->m_derivedScale * (m_initialPosition + m_position)) + m_parent->m_derivedPosition;
		else
			m_derivedPosition = m_initialPosition + m_position;

		if (m_inheritRotation)
		{
			m_derivedRotation = m_initialRotation * m_parent->m_derivedRotation * m_rotation;
			m_derivedRotation.Normalize();
		}
		else
			m_derivedRotation = m_initialRotation * m_rotation; ///FIXME: Besoin d'une normalisation ?

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

void NzNode::UpdateTransformMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_transformMatrix.MakeTransform(m_derivedPosition, m_derivedRotation, m_derivedScale);
	m_transformMatrixUpdated = true;
}
