// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Node.hpp>
#include <Nazara/Utility/Debug.hpp>

NzNode::NzNode() :
m_initialRotation(NzQuaternionf::Identity()),
m_rotation(NzQuaternionf::Identity()),
m_initialScale(NzVector3f(1.f, 1.f, 1.f)),
m_initialTranslation(NzVector3f::Zero()),
m_scale(NzVector3f(1.f, 1.f, 1.f)),
m_translation(NzVector3f::Zero()),
m_parent(nullptr),
m_derivedUpdated(false),
m_inheritRotation(true),
m_inheritScale(true),
m_inheritTranslation(true),
m_matrixUpdated(false)
{
}

NzNode::NzNode(const NzNode& node) :
m_initialRotation(node.m_initialRotation),
m_rotation(node.m_rotation),
m_initialScale(node.m_initialScale),
m_initialTranslation(node.m_initialTranslation),
m_scale(node.m_scale),
m_translation(node.m_translation),
m_parent(node.m_parent),
m_derivedUpdated(false),
m_inheritRotation(node.m_inheritRotation),
m_inheritScale(node.m_inheritScale),
m_inheritTranslation(node.m_inheritTranslation),
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

NzQuaternionf NzNode::GetDerivedRotation() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedRotation;
}

NzVector3f NzNode::GetDerivedScale() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedScale;
}

NzVector3f NzNode::GetDerivedTranslation() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	return m_derivedTranslation;
}

bool NzNode::GetInheritRotation() const
{
	return m_inheritRotation;
}

bool NzNode::GetInheritScale() const
{
	return m_inheritScale;
}

bool NzNode::GetInheritTranslation() const
{
	return m_inheritTranslation;
}

NzQuaternionf NzNode::GetInitialRotation() const
{
	return m_initialRotation;
}

NzVector3f NzNode::GetInitialScale() const
{
	return m_initialScale;
}

NzVector3f NzNode::GetInitialTranslation() const
{
	return m_initialTranslation;
}

const NzNode* NzNode::GetParent() const
{
	return m_parent;
}

NzQuaternionf NzNode::GetRotation() const
{
	return m_rotation;
}

NzVector3f NzNode::GetScale() const
{
	return m_scale;
}

NzVector3f NzNode::GetTranslation() const
{
	return m_translation;
}

const NzMatrix4f& NzNode::GetTransformMatrix() const
{
	if (!m_matrixUpdated)
		UpdateMatrix();

	return m_transformMatrix;
}

NzNode& NzNode::Interpolate(const NzNode& nodeA, const NzNode& nodeB, float interpolation)
{
	m_rotation = NzQuaternionf::Slerp(nodeA.m_rotation, nodeB.m_rotation, interpolation);
	m_scale = NzVector3f::Lerp(nodeA.m_scale, nodeB.m_scale, interpolation);
	m_translation = NzVector3f::Lerp(nodeA.m_translation, nodeB.m_translation, interpolation);

	Invalidate();
	return *this;
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

NzNode& NzNode::Translate(const NzVector3f& movement, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
		{
			if (m_parent)
				m_translation += (m_parent->GetDerivedRotation().GetInverse() * movement) / m_parent->GetDerivedScale(); // Compensation
			else
				m_translation += movement; // Rien n'affecte le node

			break;
		}

		case nzCoordSys_Local:
			m_translation += m_scale * (m_rotation * movement);
			break;
	}

	Invalidate();
	return *this;
}

NzNode& NzNode::Translate(float moveX, float moveY, float moveZ, nzCoordSys coordSys)
{
	return Translate(NzVector3f(moveX, moveY, moveZ), coordSys);
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

void NzNode::SetInheritTranslation(bool inheritTranslation)
{
	if (m_inheritTranslation != inheritTranslation)
	{
		m_inheritTranslation = inheritTranslation;

		Invalidate();
	}
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

void NzNode::SetInitialTranslation(const NzVector3f& translation)
{
	m_initialTranslation = translation;

	Invalidate();
}

void NzNode::SetInitialTranslation(float translationX, float translationY, float translationZ)
{
	m_initialTranslation.Set(translationX, translationY, translationZ);

	Invalidate();
}

void NzNode::SetParent(const NzNode* node)
{
	if (m_parent == node)
		return;

	if (m_parent)
		m_parent->RemoveChild(this);

	m_parent = node;
	if (m_parent)
		m_parent->AddChild(this);

	Invalidate();
}

void NzNode::SetParent(const NzNode& node)
{
	SetParent(&node);
}

void NzNode::SetRotation(const NzQuaternionf& rotation, nzCoordSys coordSys)
{
	// Évitons toute mauvaise surprise ..
	NzQuaternionf q(rotation);
	q.Normalize();

	switch (coordSys)
	{
		case nzCoordSys_Global:
		{
			if (m_parent)
			{
				m_rotation = q * m_parent->GetDerivedRotation().GetInverse(); ///FIXME: Vérifier si le résultat est correct
				m_rotation.Normalize();
			}
			else
				m_rotation = q;

			break;
		}

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
		{
			if (m_parent)
				m_scale = scale / m_parent->GetDerivedScale();
			else
				m_scale = scale;
			break;
		}

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

void NzNode::SetTranslation(const NzVector3f& translation, nzCoordSys coordSys)
{
	switch (coordSys)
	{
		case nzCoordSys_Global:
		{
			if (m_parent)
				m_translation = translation - m_parent->GetDerivedTranslation();
			else
				m_translation = translation;
			break;
		}

		case nzCoordSys_Local:
			m_translation = translation;
			break;
	}

	Invalidate();
}

void NzNode::SetTranslation(float translationX, float translationY, float translationZ, nzCoordSys coordSys)
{
	SetTranslation(NzVector3f(translationX, translationY, translationZ), coordSys);
}

NzNode& NzNode::operator=(const NzNode& node)
{
	SetParent(node.m_parent);

	m_inheritRotation = node.m_inheritRotation;
	m_inheritScale = node.m_inheritScale;
	m_inheritTranslation = node.m_inheritTranslation;
	m_rotation = node.m_rotation;
	m_scale = node.m_scale;
	m_translation = node.m_translation;

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

		if (m_inheritTranslation)
			m_derivedTranslation = m_parent->m_derivedRotation*(m_parent->m_derivedScale * (m_initialTranslation + m_translation)) + m_parent->m_derivedTranslation;
		else
			m_derivedTranslation = m_initialTranslation + m_translation;
	}
	else
	{
		m_derivedRotation = m_initialRotation * m_rotation;
		m_derivedScale = m_initialScale * m_scale;
		m_derivedTranslation = m_initialTranslation + m_translation;
	}

	m_derivedUpdated = true;
}

void NzNode::UpdateMatrix() const
{
	if (!m_derivedUpdated)
		UpdateDerived();

	m_transformMatrix.MakeTransform(m_derivedTranslation, m_derivedScale, m_derivedRotation);

	m_matrixUpdated = true;
}
