// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PRIMITIVELIST_HPP
#define NAZARA_PRIMITIVELIST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Primitive.hpp>
#include <Nazara/Math/Quaternion.hpp>

namespace Nz
{
	///TODO: Inline this
	class NAZARA_CORE_API PrimitiveList
	{
		public:
			PrimitiveList() = default;
			PrimitiveList(const PrimitiveList&) = default;
			PrimitiveList(PrimitiveList&&) = default;
			~PrimitiveList() = default;

			void AddBox(const Vector3f& lengths, const Vector3ui& subdivision = Vector3ui(0U), const Matrix4f& transformMatrix = Matrix4f::Identity());
			void AddBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			void AddCone(float length, float radius, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity());
			void AddCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			void AddCubicSphere(float size, unsigned int subdivision = 4, const Matrix4f& transformMatrix = Matrix4f::Identity());
			void AddCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			void AddIcoSphere(float size, unsigned int recursionLevel = 3, const Matrix4f& transformMatrix = Matrix4f::Identity());
			void AddIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			void AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix = Matrix4f::Identity());
			void AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& planeInfo);
			void AddPlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());
			void AddUVSphere(float size, unsigned int sliceCount = 4, unsigned int stackCount = 4, const Matrix4f& transformMatrix = Matrix4f::Identity());
			void AddUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation = Quaternionf::Identity());

			Primitive& GetPrimitive(std::size_t i);
			const Primitive& GetPrimitive(std::size_t i) const;
			std::size_t GetSize() const;

			PrimitiveList& operator=(const PrimitiveList&) = default;
			PrimitiveList& operator=(PrimitiveList&&) = default;

			Primitive& operator()(unsigned int i);
			const Primitive& operator()(unsigned int i) const;

		private:
			std::vector<Primitive> m_primitives;
	};
}

#endif // NAZARA_PRIMITIVELIST_HPP
