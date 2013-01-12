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

		bool GetInheritPosition() const;
		bool GetInheritRotation() const;
		bool GetInheritScale() const;
		NzVector3f GetInitialPosition() const;
		NzQuaternionf GetInitialRotation() const;
		NzVector3f GetInitialScale() const;
		const NzNode* GetParent() const;
		NzVector3f GetPosition(nzCoordSys coordSys = nzCoordSys_Global) const;
		NzQuaternionf GetRotation(nzCoordSys coordSys = nzCoordSys_Global) const;
		NzVector3f GetScale(nzCoordSys coordSys = nzCoordSys_Global) const;
		const NzMatrix4f& GetTransformMatrix() const;

		NzNode& Interpolate(const NzNode& nodeA, const NzNode& nodeB, float interpolation);

		NzNode& Move(const NzVector3f& movement, nzCoordSys coordSys = nzCoordSys_Local);
		NzNode& Move(float movementX, float movementY, float movementZ, nzCoordSys coordSys = nzCoordSys_Local);

		NzNode& Rotate(const NzQuaternionf& rotation, nzCoordSys coordSys = nzCoordSys_Local);

		NzNode& Scale(const NzVector3f& scale);
		NzNode& Scale(float scale);
		NzNode& Scale(float scaleX, float scaleY, float scaleZ);

		void SetInheritRotation(bool inheritRotation);
		void SetInheritScale(bool inheritScale);
		void SetInheritPosition(bool inheritPosition);
		void SetInitialRotation(const NzQuaternionf& quat);
		void SetInitialScale(const NzVector3f& scale);
		void SetInitialScale(float scale);
		void SetInitialScale(float scaleX, float scaleY, float scaleZ);
		void SetInitialPosition(const NzVector3f& translation);
		void SetInitialPosition(float translationX, float translationXY, float translationZ);
		void SetParent(const NzNode* node = nullptr, bool keepDerived = false);
		void SetParent(const NzNode& node, bool keepDerived = false);
		void SetPosition(const NzVector3f& translation, nzCoordSys coordSys = nzCoordSys_Local);
		void SetPosition(float translationX, float translationXY, float translationZ, nzCoordSys coordSys = nzCoordSys_Local);
		void SetRotation(const NzQuaternionf& quat, nzCoordSys coordSys = nzCoordSys_Local);
		void SetScale(const NzVector3f& scale, nzCoordSys coordSys = nzCoordSys_Local);
		void SetScale(float scale, nzCoordSys coordSys = nzCoordSys_Local);
		void SetScale(float scaleX, float scaleY, float scaleZ, nzCoordSys coordSys = nzCoordSys_Local);

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
		mutable NzVector3f m_derivedPosition;
		mutable NzVector3f m_derivedScale;
		NzVector3f m_initialPosition;
		NzVector3f m_initialScale;
		NzVector3f m_position;
		NzVector3f m_scale;
		const NzNode* m_parent;
		mutable bool m_derivedUpdated;
		bool m_inheritPosition;
		bool m_inheritRotation;
		bool m_inheritScale;
		mutable bool m_matrixUpdated;
};

#endif // NAZARA_NODE_HPP
