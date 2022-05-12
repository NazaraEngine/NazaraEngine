// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_ALGORITHM_HPP
#define NAZARA_UTILITY_ALGORITHM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/SparsePtr.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Utility/IndexIterator.hpp>

namespace Nz
{
	class Joint;
	struct VertexStruct_XYZ_Normal_UV_Tangent;
	struct VertexStruct_XYZ_Normal_UV_Tangent_Skinning;

	using MeshVertex = VertexStruct_XYZ_Normal_UV_Tangent;
	using SkeletalMeshVertex = VertexStruct_XYZ_Normal_UV_Tangent_Skinning;

	struct SkinningData
	{
		const Joint* joints;
		const SkeletalMeshVertex* inputVertex;
		MeshVertex* outputVertex;
	};

	struct VertexPointers
	{
		SparsePtr<Vector3f> normalPtr;
		SparsePtr<Vector3f> positionPtr;
		SparsePtr<Vector3f> tangentPtr;
		SparsePtr<Vector2f> uvPtr;
	};

	NAZARA_UTILITY_API Boxf ComputeAABB(SparsePtr<const Vector3f> positionPtr, UInt64 vertexCount);
	NAZARA_UTILITY_API void ComputeBoxIndexVertexCount(const Vector3ui& subdivision, UInt64* indexCount, UInt64* vertexCount);
	NAZARA_UTILITY_API UInt64 ComputeCacheMissCount(IndexIterator indices, UInt64 indexCount);
	NAZARA_UTILITY_API void ComputeConeIndexVertexCount(unsigned int subdivision, UInt64* indexCount, UInt64* vertexCount);
	NAZARA_UTILITY_API void ComputeCubicSphereIndexVertexCount(unsigned int subdivision, UInt64* indexCount, UInt64* vertexCount);
	NAZARA_UTILITY_API void ComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, UInt64* indexCount, UInt64* vertexCount);
	NAZARA_UTILITY_API void ComputePlaneIndexVertexCount(const Vector2ui& subdivision, UInt64* indexCount, UInt64* vertexCount);
	NAZARA_UTILITY_API void ComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, UInt64* indexCount, UInt64* vertexCount);

	NAZARA_UTILITY_API void GenerateBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt64 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateCone(float length, float radius, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt64 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateCubicSphere(float size, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt64 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt64 indexOffset = 0);
	NAZARA_UTILITY_API void GeneratePlane(const Vector2ui& subdivision, const Vector2f& size, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt64 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt64 indexOffset = 0);

	NAZARA_UTILITY_API void OptimizeIndices(IndexIterator indices, UInt64 indexCount);

	NAZARA_UTILITY_API void SkinPosition(const SkinningData& data, UInt64 startVertex, UInt64 vertexCount);
	NAZARA_UTILITY_API void SkinPositionNormal(const SkinningData& data, UInt64 startVertex, UInt64 vertexCount);
	NAZARA_UTILITY_API void SkinPositionNormalTangent(const SkinningData& data, UInt64 startVertex, UInt64 vertexCount);

	inline Vector3f TransformPositionTRS(const Vector3f& transformTranslation, const Quaternionf& transformRotation, const Vector3f& transformScale, const Vector3f& position);
	inline Vector3f TransformNormalTRS(const Quaternionf& transformRotation, const Vector3f& transformScale, const Vector3f& normal);
	inline Quaternionf TransformRotationTRS(const Quaternionf& transformRotation, const Vector3f& transformScale, const Quaternionf& rotation);
	inline Vector3f TransformScaleTRS(const Vector3f& transformScale, const Vector3f& scale);
	inline void TransformTRS(const Vector3f& transformTranslation, const Quaternionf& transformRotation, const Vector3f& transformScale, Vector3f& position, Quaternionf& rotation, Vector3f& scale);
	inline void TransformVertices(VertexPointers vertexPointers, UInt64 vertexCount, const Matrix4f& matrix);

	template<typename T> constexpr ComponentType ComponentTypeId();
	template<typename T> constexpr ComponentType GetComponentTypeOf();
}

#include <Nazara/Utility/Algorithm.inl>

#endif // NAZARA_UTILITY_ALGORITHM_HPP
