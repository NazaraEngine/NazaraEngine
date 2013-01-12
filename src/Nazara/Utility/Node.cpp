// Copyright (C) 2012 Jérôme Leclercq
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
m_matrixUpdated(false)
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
m_matrixUpdated(false)
{
	if (m_parent)
		m_parent->AddChild(this);
}

NzNode::~NzNode()
{
	for (NzNode* child : m_childs)
		child->m_parent = nullptr;

	if (m_parent)
		m_parent->RemoveChild(this);
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
	if (!m_matrixUpdated)
		UpdateMatrix();

	return m_transformMatrix;
}

NzNode& NzNode::Interpolate(const NzNode& nodeA, const NzNode& nodeB, float interpolation)
{
	m_position = NzVector3f::Lerp(nodeA.m_position, nodeB.m_position, interpolation);
	m_rotation = NzQuaternionf::Slerp(nodeA.m_rotation, nodeB.m_rotation, interpolation);
	m_scale = NzVector3f::Lerp(nodeA.m_scale, nodeB.m_scale, interpolation);

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

				m_position += (m_parent->m_derivedRotation.GetInverse() * movement) / m_parent->m_derivedPosition; // Compensation
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

			m_rotation *= m_derivedRotation.GetInverse() * q * m_derivedRotation;
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

void NzNode::SetParent(const NzNode* node, bool keepDerived)
{
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
				NzQuaternionf rot(m_initialRotation * m_parent->GetRotation());

				m_rotation = rot.GetInverse() * q; ///FIXME: Vérifier si le résultat est correct
				m_rotation.Normalize();
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

NzNode& NzNode::operator=(const NzNode& node)
{
	SetParent(node.m_parent);

	m_inheritPosition = node.m_inheritPosition;
	m_inheritRotation = node.m_inheritRotation;
	m_inheritScale = node.m_inheritScale;
	m_position = node.m_position;
	m_rotation = node.m_rotation;
	m_scale = node.m_scale;

	Invalidate();

	return *this;
}

void NzNode::AddChild(NzNode* node) const
{
	auto pair = m_childs.insert(node);

	if (pair.second)
		node->Invalidate();
	#ifdef NAZARA_DEBUG
	else
		NazaraWarning("Child already in set");
	#endif
}

void NzNode::Invalidate()
{
	m_derivedUpdated = false;
	m_matrixUpdated = false;

	for (NzNode* node : m_childs)
		node->Invalidate();
}

void NzNode::RemoveChild(NzNode* node) const
{
	#ifdef NAZARA_DEBUG
	if (m_childs.erase(node) == 0)
		NazaraWarning("Child not found in set");
	#else
	m_childs.erase(node);
	#endif
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

void NzNode::UpdateMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_transformMatrix.MakeTransform(m_derivedPosition, m_derivedScale, m_derivedRotation);

	m_matrixUpdated = true;
}
