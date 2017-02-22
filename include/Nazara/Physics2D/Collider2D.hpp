// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLLIDER2D_HPP
#define NAZARA_COLLIDER2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Physics2D/Config.hpp>
#include <Nazara/Physics2D/Enums.hpp>
#include <vector>

struct cpShape;
struct cpSpace;

namespace Nz
{
	class Collider2D;
	class RigidBody2D;

	using Collider2DConstRef = ObjectRef<const Collider2D>;
	using Collider2DLibrary = ObjectLibrary<Collider2D>;
	using Collider2DRef = ObjectRef<Collider2D>;

	class NAZARA_PHYSICS2D_API Collider2D : public RefCounted
	{
		friend Collider2DLibrary;
		friend RigidBody2D;

		public:
			inline Collider2D();
			Collider2D(const Collider2D&) = delete;
			Collider2D(Collider2D&&) = delete;
			virtual ~Collider2D();

			virtual float ComputeInertialMatrix(float mass) const = 0;

			inline unsigned int GetCollisionId() const;

			virtual ColliderType2D GetType() const = 0;

			inline bool IsTrigger() const;

			inline void SetCollisionId(unsigned long typeId);
			inline void SetTrigger(bool trigger);

			Collider2D& operator=(const Collider2D&) = delete;
			Collider2D& operator=(Collider2D&&) = delete;

			// Signals:
			NazaraSignal(OnColliderRelease, const Collider2D* /*collider*/);

		protected:
			virtual std::vector<cpShape*> CreateShapes(RigidBody2D* body) const = 0;

			bool m_trigger;
			unsigned int m_collisionId;

		private:
			virtual std::vector<cpShape*> GenerateShapes(RigidBody2D* body) const;

			static Collider2DLibrary::LibraryMap s_library;
	};

	class BoxCollider2D;

	using BoxCollider2DConstRef = ObjectRef<const BoxCollider2D>;
	using BoxCollider2DRef = ObjectRef<BoxCollider2D>;

	class NAZARA_PHYSICS2D_API BoxCollider2D : public Collider2D
	{
		public:
			BoxCollider2D(const Vector2f& size, float radius = 0.f);
			BoxCollider2D(const Rectf& rect, float radius = 0.f);

			float ComputeInertialMatrix(float mass) const override;

			inline const Rectf& GetRect() const;
			inline Vector2f GetSize() const;
			ColliderType2D GetType() const override;

			template<typename... Args> static BoxCollider2DRef New(Args&&... args);

		private:
			std::vector<cpShape*> CreateShapes(RigidBody2D* body) const override;

			Rectf m_rect;
			float m_radius;
	};

	class CircleCollider2D;

	using CircleCollider2DConstRef = ObjectRef<const CircleCollider2D>;
	using CircleCollider2DRef = ObjectRef<CircleCollider2D>;

	class NAZARA_PHYSICS2D_API CircleCollider2D : public Collider2D
	{
		public:
			CircleCollider2D(float radius, const Vector2f& offset = Vector2f::Zero());

			float ComputeInertialMatrix(float mass) const override;

			inline float GetRadius() const;
			ColliderType2D GetType() const override;

			template<typename... Args> static CircleCollider2DRef New(Args&&... args);

		private:
			std::vector<cpShape*> CreateShapes(RigidBody2D* body) const override;

			Vector2f m_offset;
			float m_radius;
	};

	class NullCollider2D;

	using NullCollider2DConstRef = ObjectRef<const NullCollider2D>;
	using NullCollider2DRef = ObjectRef<NullCollider2D>;

	class NAZARA_PHYSICS2D_API NullCollider2D : public Collider2D
	{
		public:
			NullCollider2D() = default;

			float ComputeInertialMatrix(float mass) const override;

			ColliderType2D GetType() const override;

			template<typename... Args> static NullCollider2DRef New(Args&&... args);

		private:
			std::vector<cpShape*> CreateShapes(RigidBody2D* body) const override;
	};

	class SegmentCollider2D;

	using SegmentCollider2DConstRef = ObjectRef<const SegmentCollider2D>;
	using SegmentCollider2DRef = ObjectRef<SegmentCollider2D>;

	class NAZARA_PHYSICS2D_API SegmentCollider2D : public Collider2D
	{
		public:
			inline SegmentCollider2D(const Vector2f& first, const Vector2f& second, float thickness = 1.f);

			float ComputeInertialMatrix(float mass) const override;

			inline const Vector2f& GetFirstPoint() const;
			inline float GetLength() const;
			inline const Vector2f& GetSecondPoint() const;
			ColliderType2D GetType() const override;

			template<typename... Args> static SegmentCollider2DRef New(Args&&... args);

		private:
			std::vector<cpShape*> CreateShapes(RigidBody2D* body) const override;

			Vector2f m_first;
			Vector2f m_second;
			float m_thickness;
	};
}

#include <Nazara/Physics2D/Collider2D.inl>

#endif // NAZARA_COLLIDER2D_HPP
