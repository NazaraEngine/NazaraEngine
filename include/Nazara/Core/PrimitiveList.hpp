// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PRIMITIVELIST_HPP
#define NAZARA_CORE_PRIMITIVELIST_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Math/Quaternion.hpp>

namespace Nz
{
	class PrimitiveList
	{
		public:
			PrimitiveList() = default;
			inline PrimitiveList(const Primitive& primitive);
			inline PrimitiveList(std::initializer_list<Primitive> primitives);

			inline void Add(const Primitive& primitive);
			inline void AddBox(const Vector3f& lengths, const Vector3ui& subdivision = Vector3ui(0U), const Matrix4f& transformMatrix = Matrix4f::Identity());
			inline void AddBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			inline void AddCone(float length, float radius, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity());
			inline void AddCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			inline void AddCubicSphere(float size, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity());
			inline void AddCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			inline void AddIcoSphere(float size, unsigned int recursionLevel = 3, const Matrix4f& transformMatrix = Matrix4f::Identity());
			inline void AddIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			inline void AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix = Matrix4f::Identity());
			inline void AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& planeInfo);
			inline void AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			inline void AddUVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const Matrix4f& transformMatrix = Matrix4f::Identity());
			inline void AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());

			inline Primitive& GetPrimitive(std::size_t i);
			inline const Primitive& GetPrimitive(std::size_t i) const;
			inline std::size_t GetSize() const;

			inline Primitive& operator[](std::size_t i);
			inline const Primitive& operator[](std::size_t i) const;

		private:
			std::vector<Primitive> m_primitives;
	};
}

#include <Nazara/Core/PrimitiveList.inl>

#endif // NAZARA_CORE_PRIMITIVELIST_HPP
