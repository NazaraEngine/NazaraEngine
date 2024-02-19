// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_NODE_HPP
#define NAZARA_CORE_NODE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/Signal.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API Node
	{
		public:
			enum class Invalidation;

			inline Node(const Vector3f& translation = Vector3f::Zero(), const Quaternionf& rotation = Quaternionf::Identity(), const Vector3f& scale = Vector3f::Unit());
			inline Node(const Node& node);
			inline Node(Node&& node) noexcept;
			virtual ~Node();

			inline bool DoesInheritPosition() const;
			inline bool DoesInheritRotation() const;
			inline bool DoesInheritScale() const;

			inline void EnsureGlobalsUpdate() const;
			inline void EnsureTransformMatrixUpdate() const;

			inline Vector3f GetBackward() const;
			inline const std::vector<Node*>& GetChilds() const;
			inline Vector3f GetDown() const;
			inline Vector3f GetForward() const;
			inline const Vector3f& GetGlobalPosition() const;
			inline const Quaternionf& GetGlobalRotation() const;
			inline const Vector3f& GetGlobalScale() const;
			inline Vector3f GetLeft() const;
			inline const Node* GetParent() const;
			inline const Vector3f& GetPosition() const;
			inline Vector3f GetRight() const;
			inline const Quaternionf& GetRotation() const;
			inline const Vector3f& GetScale() const;
			inline const Matrix4f& GetTransformMatrix() const;
			inline Vector3f GetUp() const;

			inline bool HasChilds() const;

			inline void Invalidate(Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline Node& Interpolate(const Node& nodeA, const Node& nodeB, float interpolation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& InterpolateGlobal(const Node& nodeA, const Node& nodeB, float interpolation, Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline Node& Move(const Vector2f& movement, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& Move(const Vector3f& movement, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& MoveGlobal(const Vector2f& movement, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& MoveGlobal(const Vector3f& movement, Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline Node& Rotate(const Quaternionf& rotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& RotateGlobal(const Quaternionf& rotation, Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline Node& Scale(float scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& Scale(const Vector2f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline Node& Scale(const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);

			inline void SetGlobalPosition(const Vector2f& position, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalPosition(const Vector3f& position, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalRotation(const Quaternionf& rotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalScale(const Vector2f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalScale(const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalTransform(const Vector3f& position, const Quaternionf& rotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalTransform(const Vector3f& position, const Quaternionf& rotation, const Vector2f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetGlobalTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInheritPosition(bool inheritPosition, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInheritRotation(bool inheritRotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetInheritScale(bool inheritScale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetParent(const Node* node = nullptr, bool keepDerived = false, Invalidation invalidation = Invalidation::InvalidateRecursively);
			void SetParent(const Node& node, bool keepDerived = false, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetPosition(const Vector2f& position, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetPosition(const Vector3f& position, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetRotation(const Quaternionf& rotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetScale(float scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetScale(const Vector2f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetScale(const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetTransform(const Vector3f& position, const Quaternionf& rotation, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector2f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
			inline void SetTransform(const Vector3f& position, const Quaternionf& rotation, const Vector3f& scale, Invalidation invalidation = Invalidation::InvalidateRecursively);
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
			mutable Quaternionf m_globalRotation;
			Quaternionf m_rotation;
			mutable Vector3f m_globalPosition;
			mutable Vector3f m_globalScale;
			Vector3f m_position;
			Vector3f m_scale;
			const Node* m_parent;
			mutable bool m_derivedUpdated;
			bool m_doesInheritPosition;
			bool m_doesInheritRotation;
			bool m_doesInheritScale;
			mutable bool m_transformMatrixUpdated;
	};
}

#include <Nazara/Core/Node.inl>

#endif // NAZARA_CORE_NODE_HPP
