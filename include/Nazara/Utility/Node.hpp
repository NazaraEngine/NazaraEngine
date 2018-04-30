// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NODE_HPP
#define NAZARA_NODE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API Node
	{
		public:
			Node();
			Node(const Node& node);
			virtual ~Node();

			void EnsureDerivedUpdate() const;
			void EnsureTransformMatrixUpdate() const;

			virtual Vector3f GetBackward() const;
			const std::vector<Node*>& GetChilds() const;
			virtual Vector3f GetDown() const;
			virtual Vector3f GetForward() const;
			bool GetInheritPosition() const;
			bool GetInheritRotation() const;
			bool GetInheritScale() const;
			Vector3f GetInitialPosition() const;
			Quaternionf GetInitialRotation() const;
			Vector3f GetInitialScale() const;
			virtual Vector3f GetLeft() const;
			virtual NodeType GetNodeType() const;
			const Node* GetParent() const;
			Vector3f GetPosition(CoordSys coordSys = CoordSys_Global) const;
			virtual Vector3f GetRight() const;
			Quaternionf GetRotation(CoordSys coordSys = CoordSys_Global) const;
			Vector3f GetScale(CoordSys coordSys = CoordSys_Global) const;
			const Matrix4f& GetTransformMatrix() const;
			virtual Vector3f GetUp() const;

			bool HasChilds() const;

			Node& Interpolate(const Node& nodeA, const Node& nodeB, float interpolation, CoordSys coordSys = CoordSys_Global);

			Node& Move(const Vector3f& movement, CoordSys coordSys = CoordSys_Local);
			Node& Move(float movementX, float movementY, float movementZ = 0.f, CoordSys coordSys = CoordSys_Local);

			Node& Rotate(const Quaternionf& rotation, CoordSys coordSys = CoordSys_Local);

			Node& Scale(const Vector3f& scale);
			Node& Scale(float scale);
			Node& Scale(float scaleX, float scaleY, float scaleZ = 1.f);

			void SetInheritRotation(bool inheritRotation);
			void SetInheritScale(bool inheritScale);
			void SetInheritPosition(bool inheritPosition);
			void SetInitialRotation(const Quaternionf& quat);
			void SetInitialScale(const Vector3f& scale);
			void SetInitialScale(float scale);
			void SetInitialScale(float scaleX, float scaleY, float scaleZ = 1.f);
			void SetInitialPosition(const Vector3f& translation);
			void SetInitialPosition(float translationX, float translationXY, float translationZ = 0.f);
			void SetParent(const Node* node = nullptr, bool keepDerived = false);
			void SetParent(const Node& node, bool keepDerived = false);
			void SetPosition(const Vector3f& translation, CoordSys coordSys = CoordSys_Local);
			void SetPosition(float translationX, float translationY, float translationZ = 0.f, CoordSys coordSys = CoordSys_Local);
			void SetRotation(const Quaternionf& quat, CoordSys coordSys = CoordSys_Local);
			void SetScale(const Vector3f& scale, CoordSys coordSys = CoordSys_Local);
			void SetScale(float scale, CoordSys coordSys = CoordSys_Local);
			void SetScale(float scaleX, float scaleY, float scaleZ = 1.f, CoordSys coordSys = CoordSys_Local);
			void SetTransformMatrix(const Matrix4f& matrix);

			// Local -> global
			Vector3f ToGlobalPosition(const Vector3f& localPosition) const;
			Quaternionf ToGlobalRotation(const Quaternionf& localRotation) const;
			Vector3f ToGlobalScale(const Vector3f& localScale) const;

			// Global -> local
			Vector3f ToLocalPosition(const Vector3f& globalPosition) const;
			Quaternionf ToLocalRotation(const Quaternionf& globalRotation) const;
			Vector3f ToLocalScale(const Vector3f& globalScale) const;

			Node& operator=(const Node& node);

			// Signals:
			NazaraSignal(OnNodeInvalidation, const Node* /*node*/);
			NazaraSignal(OnNodeNewParent, const Node* /*node*/, const Node* /*parent*/);
			NazaraSignal(OnNodeRelease, const Node* /*node*/);

		protected:
			void AddChild(Node* node) const;
			virtual void InvalidateNode();
			virtual void OnParenting(const Node* parent);
			void RemoveChild(Node* node) const;
			virtual void UpdateDerived() const;
			virtual void UpdateTransformMatrix() const;

			mutable std::vector<Node*> m_childs;
			mutable Matrix4f m_transformMatrix;
			mutable Quaternionf m_derivedRotation;
			Quaternionf m_initialRotation;
			Quaternionf m_rotation;
			mutable Vector3f m_derivedPosition;
			mutable Vector3f m_derivedScale;
			Vector3f m_initialPosition;
			Vector3f m_initialScale;
			Vector3f m_position;
			Vector3f m_scale;
			const Node* m_parent;
			mutable bool m_derivedUpdated;
			bool m_inheritPosition;
			bool m_inheritRotation;
			bool m_inheritScale;
			mutable bool m_transformMatrixUpdated;
	};
}

#endif // NAZARA_NODE_HPP
