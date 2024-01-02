// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_NODE_HPP
#define NAZARA_UTILITY_NODE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <NazaraUtils/Signal.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API Node
	{
		public:
			enum class Invalidation;

			inline Node(const Vector3f& translation = Vector3f::Zero(), const Quaternionf& rotation = Quaternionf::Identity(), const Vector3f& scale = Vector3f::Unit());
			inline Node(const Node& node);
			inline Node(Node&& node) noexcept;
			virtual ~Node();

			inline void EnsureDerivedUpdate() const;
			inline void EnsureTransformMatrixUpdate() const;

			inline Vector3f GetBackward() const;
			inline const std::vector<Node*>& GetChilds() const;
			inline Vector3f GetDown() const;
			inline Vector3f GetForward() const;
			inline bool GetInheritPosition() const;
			inline bool GetInheritRotation() const;
			inline bool GetInheritScale() const;
			inline Vector3f GetInitialPosition() const;
			inline Quaternionf GetInitialRotation() const;
			inline Vector3f GetInitialScale() const;
			inline Vector3f GetLeft() const;
			virtual NodeType GetNodeType() const;
			inline const Node* GetParent() const;
			inline Vector3f GetPosition(CoordSys coordSys = CoordSys::Local) const;
			inline Vector3f GetRight() const;
			inline Quaternionf GetRotation(CoordSys coordSys = CoordSys::Local) const;
			inline Vector3f GetScale(CoordSys coordSys = CoordSys::Local) const;
			inline const Matrix4f& GetTransformMatrix() const;
			inline Vector3f GetUp() const;

			inline bool HasChilds() const;

			inline void Invalidate(Invalidation invalidation = Invalidation::InvalidateRecursively);

			Node& Interpolate(const Node& nodeA, const Node& nodeB, float interpolation, CoordSys coordSys = CoordSys::Global, Invalidation invalidation = Invalidation::InvalidateRecursively);

			Node& Move(const Vector3f& movement, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& Move(float movementX, float movementY, float movementZ = 0.f, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);

			Node& Rotate(const Quaternionf& rotation, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline Node& Scale(const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& Scale(float scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& Scale(float scaleX, float scaleY, float scaleZ = 1.f, Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline void SetInheritPosition(bool inheritPosition, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInheritRotation(bool inheritRotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInheritScale(bool inheritScale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInitialPosition(const Vector3f& translation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInitialPosition(float translationX, float translationXY, float translationZ = 0.f, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInitialRotation(const Quaternionf& quat, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInitialScale(const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInitialScale(float scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInitialScale(float scaleX, float scaleY, float scaleZ = 1.f, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetParent(const Node* node = nullptr, bool keepDerived = false, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetParent(const Node& node, bool keepDerived = false, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetPosition(const Vector3f& translation, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetPosition(float translationX, float translationY, float translationZ = 0.f, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetRotation(const Quaternionf& rotation, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetScale(const Vector2f& scale, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetScale(const Vector3f& scale, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetScale(float scale, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetScale(float scaleX, float scaleY, float scaleZ = 1.f, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetTransform(const Vector3f& position, const Quaternionf& rotation, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale, CoordSys coordSys = CoordSys::Local, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetTransformMatrix(const Matrix4f& matrix, Invalidation invalidation = Invalidation::InvalidateRecursively);

			// Local -> global
			inline Vector3f ToGlobalPosition(const Vector3f& localPosition) const;
			inline Quaternionf ToGlobalRotation(const Quaternionf& localRotation) const;
			inline Vector3f ToGlobalScale(const Vector3f& localScale) const;

			// Global -> local
			inline Vector3f ToLocalPosition(const Vector3f& globalPosition) const;
			inline Quaternionf ToLocalRotation(const Quaternionf& globalRotation) const;
			inline Vector3f ToLocalScale(const Vector3f& globalScale) const;

			inline Node& operator=(const Node& node);
			inline Node& operator=(Node&& node) noexcept;

			// Signals:
			NazaraSignal(OnNodeInvalidation, const Node* /*node*/);
			NazaraSignal(OnNodeNewParent, const Node* /*node*/, const Node* /*parent*/);
			NazaraSignal(OnNodeRelease, const Node* /*node*/);

			enum class Invalidation
			{
				InvalidateRecursively,
				InvalidateNodeOnly,
				DontInvalidate
			};

		protected:
			inline void AddChild(Node* node) const;
			virtual void InvalidateNode(Invalidation invalidation);
			virtual void OnParenting(const Node* parent);
			inline void RemoveChild(Node* node) const;
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

#include <Nazara/Utility/Node.inl>

#endif // NAZARA_UTILITY_NODE_HPP
