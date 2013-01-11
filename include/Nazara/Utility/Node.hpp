// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NODE_HPP
#define NAZARA_NODE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <set>

class NAZARA_API NzNode
{
	public:
		NzNode();
		NzNode(const NzNode& node);
		virtual ~NzNode();

		NzQuaternionf GetDerivedRotation() const;
		NzVector3f GetDerivedScale() const;
		NzVector3f GetDerivedTranslation() const;
		bool GetInheritRotation() const;
		bool GetInheritScale() const;
		bool GetInheritTranslation() const;
		NzQuaternionf GetInitialRotation() const;
		NzVector3f GetInitialScale() const;
		NzVector3f GetInitialTranslation() const;
		const NzNode* GetParent() const;
		NzQuaternionf GetRotation() const;
		NzVector3f GetScale() const;
		NzVector3f GetTranslation() const;
		const NzMatrix4f& GetTransformMatrix() const;

		NzNode& Interpolate(const NzNode& nodeA, const NzNode& nodeB, float interpolation);

		NzNode& Rotate(const NzQuaternionf& rotation, nzCoordSys coordSys = nzCoordSys_Local);

		NzNode& Scale(const NzVector3f& scale);
		NzNode& Scale(float scale);
		NzNode& Scale(float scaleX, float scaleY, float scaleZ);

		NzNode& Translate(const NzVector3f& movement, nzCoordSys coordSys = nzCoordSys_Local);
		NzNode& Translate(float movementX, float movementY, float movementZ, nzCoordSys coordSys = nzCoordSys_Local);

		void SetInheritRotation(bool inheritRotation);
		void SetInheritScale(bool inheritScale);
		void SetInheritTranslation(bool inheritTranslation);
		void SetInitialRotation(const NzQuaternionf& quat);
		void SetInitialScale(const NzVector3f& scale);
		void SetInitialScale(float scale);
		void SetInitialScale(float scaleX, float scaleY, float scaleZ);
		void SetInitialTranslation(const NzVector3f& translation);
		void SetInitialTranslation(float translationX, float translationXY, float translationZ);
		void SetParent(const NzNode* node = nullptr, bool keepDerived = false);
		void SetParent(const NzNode& node, bool keepDerived = false);
		void SetRotation(const NzQuaternionf& quat, nzCoordSys coordSys = nzCoordSys_Local);
		void SetScale(const NzVector3f& scale, nzCoordSys coordSys = nzCoordSys_Local);
		void SetScale(float scale, nzCoordSys coordSys = nzCoordSys_Local);
		void SetScale(float scaleX, float scaleY, float scaleZ, nzCoordSys coordSys = nzCoordSys_Local);
		void SetTranslation(const NzVector3f& translation, nzCoordSys coordSys = nzCoordSys_Local);
		void SetTranslation(float translationX, float translationXY, float translationZ, nzCoordSys coordSys = nzCoordSys_Local);

		NzNode& operator=(const NzNode& node);

	protected:
		void AddChild(NzNode* node) const;
		void Invalidate();
		void RemoveChild(NzNode* node) const;
		void UpdateDerived() const;
		virtual void UpdateMatrix() const;

		mutable std::set<NzNode*> m_childs;
		mutable NzMatrix4f m_transformMatrix;
		mutable NzQuaternionf m_derivedRotation;
		NzQuaternionf m_initialRotation;
		NzQuaternionf m_rotation;
		NzString m_name;
		mutable NzVector3f m_derivedTranslation;
		mutable NzVector3f m_derivedScale;
		NzVector3f m_initialScale;
		NzVector3f m_initialTranslation;
		NzVector3f m_scale;
		NzVector3f m_translation;
		const NzNode* m_parent;
		mutable bool m_derivedUpdated;
		bool m_inheritRotation;
		bool m_inheritScale;
		bool m_inheritTranslation;
		mutable bool m_matrixUpdated;
};

#endif // NAZARA_NODE_HPP
