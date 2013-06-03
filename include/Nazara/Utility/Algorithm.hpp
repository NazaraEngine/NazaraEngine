// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_UTILITY_HPP
#define NAZARA_ALGORITHM_UTILITY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Mesh.hpp>

NAZARA_API void NzComputeBoxIndexVertexCount(const NzVector3ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputeCubicSphereIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputePlaneIndexVertexCount(const NzVector2ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount);
NAZARA_API void NzComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, unsigned int* indexCount, unsigned int* vertexCount);

///TODO: Itérateur sur les indices
NAZARA_API void NzGenerateBox(const NzBoxf& box, const NzVector3ui& subdivision, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGeneratePlane(const NzVector2ui& subdivision, const NzVector3f& position, const NzVector3f& normal, const NzVector2f& size, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);
NAZARA_API void NzGenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb = nullptr, unsigned int indexOffset = 0);

inline void NzTransformVertex(NzMeshVertex* vertex, const NzMatrix4f& matrix);
inline void NzTransformVertices(NzMeshVertex* vertices, unsigned int vertexCount, const NzMatrix4f& matrix);

#include <Nazara/Utility/Algorithm.inl>

#endif // NAZARA_ALGORITHM_UTILITY_HPP
