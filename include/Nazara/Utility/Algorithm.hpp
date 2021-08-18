// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_UTILITY_HPP
#define NAZARA_ALGORITHM_UTILITY_HPP

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

	NAZARA_UTILITY_API Boxf ComputeAABB(SparsePtr<const Vector3f> positionPtr, unsigned int vertexCount);
	NAZARA_UTILITY_API void ComputeBoxIndexVertexCount(const Vector3ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount);
	NAZARA_UTILITY_API unsigned int ComputeCacheMissCount(IndexIterator indices, unsigned int indexCount);
	NAZARA_UTILITY_API void ComputeConeIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount);
	NAZARA_UTILITY_API void ComputeCubicSphereIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount);
	NAZARA_UTILITY_API void ComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, unsigned int* indexCount, unsigned int* vertexCount);
	NAZARA_UTILITY_API void ComputePlaneIndexVertexCount(const Vector2ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount);
	NAZARA_UTILITY_API void ComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, unsigned int* indexCount, unsigned int* vertexCount);

	NAZARA_UTILITY_API void GenerateBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, unsigned int indexOffset = 0);
	NAZARA_UTILITY_API void GenerateCone(float length, float radius, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, unsigned int indexOffset = 0);
	NAZARA_UTILITY_API void GenerateCubicSphere(float size, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, unsigned int indexOffset = 0);
	NAZARA_UTILITY_API void GenerateIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, unsigned int indexOffset = 0);
	NAZARA_UTILITY_API void GeneratePlane(const Vector2ui& subdivision, const Vector2f& size, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, unsigned int indexOffset = 0);
	NAZARA_UTILITY_API void GenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, unsigned int indexOffset = 0);

	NAZARA_UTILITY_API void OptimizeIndices(IndexIterator indices, unsigned int indexCount);

	NAZARA_UTILITY_API void SkinPosition(const SkinningData& data, unsigned int startVertex, unsigned int vertexCount);
	NAZARA_UTILITY_API void SkinPositionNormal(const SkinningData& data, unsigned int startVertex, unsigned int vertexCount);
	NAZARA_UTILITY_API void SkinPositionNormalTangent(const SkinningData& data, unsigned int startVertex, unsigned int vertexCount);

	NAZARA_UTILITY_API void TransformVertices(VertexPointers vertexPointers, unsigned int vertexCount, const Matrix4f& matrix);

	template<typename T> constexpr ComponentType ComponentTypeId();
	template<typename T> constexpr ComponentType GetComponentTypeOf();
}
#include <Nazara/Utility/Algorithm.inl>

#endif // NAZARA_ALGORITHM_UTILITY_HPP
