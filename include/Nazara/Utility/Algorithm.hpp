// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_ALGORITHM_HPP
#define NAZARA_UTILITY_ALGORITHM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <NazaraUtils/SparsePtr.hpp>

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
		SparsePtr<const Vector3f> inputPositions;
		SparsePtr<const Vector3f> inputNormals;
		SparsePtr<const Vector3f> inputTangents;
		SparsePtr<const Vector4i32> inputJointIndices;
		SparsePtr<const Vector4f> inputJointWeights;
		SparsePtr<const Vector2f> inputUv;
		SparsePtr<Vector3f> outputNormals;
		SparsePtr<Vector3f> outputPositions;
		SparsePtr<Vector3f> outputTangents;
		SparsePtr<Vector2f> outputUv;
	};

	struct VertexPointers
	{
		SparsePtr<Vector3f> normalPtr;
		SparsePtr<Vector3f> positionPtr;
		SparsePtr<Vector3f> tangentPtr;
		SparsePtr<Vector2f> uvPtr;
	};

	NAZARA_UTILITY_API Boxf ComputeAABB(SparsePtr<const Vector3f> positionPtr, UInt32 vertexCount);
	NAZARA_UTILITY_API void ComputeBoxIndexVertexCount(const Vector3ui& subdivision, UInt32* indexCount, UInt32* vertexCount);
	NAZARA_UTILITY_API UInt32 ComputeCacheMissCount(IndexIterator indices, UInt32 indexCount);
	NAZARA_UTILITY_API void ComputeConeIndexVertexCount(unsigned int subdivision, UInt32* indexCount, UInt32* vertexCount);
	NAZARA_UTILITY_API void ComputeCubicSphereIndexVertexCount(unsigned int subdivision, UInt32* indexCount, UInt32* vertexCount);
	NAZARA_UTILITY_API void ComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, UInt32* indexCount, UInt32* vertexCount);
	NAZARA_UTILITY_API void ComputePlaneIndexVertexCount(const Vector2ui& subdivision, UInt32* indexCount, UInt32* vertexCount);
	NAZARA_UTILITY_API void ComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, UInt32* indexCount, UInt32* vertexCount);

	NAZARA_UTILITY_API void GenerateBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt32 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateCone(float length, float radius, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt32 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateCubicSphere(float size, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt32 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt32 indexOffset = 0);
	NAZARA_UTILITY_API void GeneratePlane(const Vector2ui& subdivision, const Vector2f& size, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt32 indexOffset = 0);
	NAZARA_UTILITY_API void GenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb = nullptr, UInt32 indexOffset = 0);

	NAZARA_UTILITY_API void OptimizeIndices(IndexIterator indices, UInt32 indexCount);

	NAZARA_UTILITY_API void SkinLinearBlend(const SkinningData& data, UInt32 startVertex, UInt32 vertexCount);

	inline Vector3f TransformPositionTRS(const Vector3f& transformTranslation, const Quaternionf& transformRotation, const Vector3f& transformScale, const Vector3f& position);
	inline Vector3f TransformNormalTRS(const Quaternionf& transformRotation, const Vector3f& transformScale, const Vector3f& normal);
	inline Quaternionf TransformRotationTRS(const Quaternionf& transformRotation, const Vector3f& transformScale, const Quaternionf& rotation);
	inline Vector3f TransformScaleTRS(const Vector3f& transformScale, const Vector3f& scale);
	inline void TransformTRS(const Vector3f& transformTranslation, const Quaternionf& transformRotation, const Vector3f& transformScale, Vector3f& position, Quaternionf& rotation, Vector3f& scale);
	inline void TransformVertices(VertexPointers vertexPointers, UInt32 vertexCount, const Matrix4f& matrix);

	template<typename T> constexpr ComponentType ComponentTypeId();
	template<typename T> constexpr ComponentType GetComponentTypeOf();
}

#include <Nazara/Utility/Algorithm.inl>

#endif // NAZARA_UTILITY_ALGORITHM_HPP
