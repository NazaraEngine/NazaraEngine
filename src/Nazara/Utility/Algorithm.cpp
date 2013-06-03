// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Math/Basic.hpp>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	class IcoSphereBuilder
	{
		public:
			IcoSphereBuilder(const NzMatrix4f& matrix) :
			m_matrix(matrix)
			{
			}

			void Generate(float size, unsigned int recursionLevel, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb, unsigned int indexOffset)
			{
				// Grandement inspiré de http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
				const float t = (1.f + 2.236067f)/2.f;

				m_cache.clear();
				m_size = size;
				m_vertices = vertices;
				m_vertexIndex = 0;

				// Sommets de base
				AddVertex({-1.f,  t, 0.f});
				AddVertex({ 1.f,  t, 0.f});
				AddVertex({-1.f, -t, 0.f});
				AddVertex({ 1.f, -t, 0.f});

				AddVertex({0.f, -1.f,  t});
				AddVertex({0.f,  1.f,  t});
				AddVertex({0.f, -1.f, -t});
				AddVertex({0.f,  1.f, -t});

				AddVertex({ t, 0.f, -1.f});
				AddVertex({ t, 0.f,  1.f});
				AddVertex({-t, 0.f, -1.f});
				AddVertex({-t, 0.f,  1.f});

				std::vector<NzVector3ui> triangles;
				triangles.reserve(20 * NzIntegralPow(4, recursionLevel));

				// Cinq triangles autour du premier point
				triangles.push_back({0, 11,  5});
				triangles.push_back({0,  5,  1});
				triangles.push_back({0,  1,  7});
				triangles.push_back({0,  7, 10});
				triangles.push_back({0, 10, 11});

				// Cinq faces adjaçentes
				triangles.push_back({ 1,  5,  9});
				triangles.push_back({ 5, 11,  4});
				triangles.push_back({11, 10,  2});
				triangles.push_back({10,  7,  6});
				triangles.push_back({ 7,  1,  8});

				// Cinq triangles autour du troisième point
				triangles.push_back({3, 9, 4});
				triangles.push_back({3, 4, 2});
				triangles.push_back({3, 2, 6});
				triangles.push_back({3, 6, 8});
				triangles.push_back({3, 8, 9});

				// Cinq faces adjaçentes
				triangles.push_back({4, 9,  5});
				triangles.push_back({2, 4, 11});
				triangles.push_back({6, 2, 10});
				triangles.push_back({8, 6,  7});
				triangles.push_back({9, 8,  1});

				// Et maintenant on affine la sphère
				for (unsigned int i = 0; i < recursionLevel; ++i)
				{
					for (NzVector3ui& triangle : triangles)
					{
						unsigned int a = GetMiddleVertex(triangle.x, triangle.y);
						unsigned int b = GetMiddleVertex(triangle.y, triangle.z);
						unsigned int c = GetMiddleVertex(triangle.z, triangle.x);

						triangles.push_back({triangle.x, a, c});
						triangles.push_back({triangle.y, b, a});
						triangles.push_back({triangle.z, c, b});

						triangle.Set(a, b, c); // Réutilisation du triangle
					}
				}

				for (const NzVector3ui& triangle : triangles)
				{
					*indices++ = triangle.x + indexOffset;
					*indices++ = triangle.y + indexOffset;
					*indices++ = triangle.z + indexOffset;
				}

				if (aabb)
				{
					NzVector3f totalSize = size * m_matrix.GetScale();
					aabb->Set(-totalSize, totalSize);
				}
			}

			unsigned int AddVertex(const NzVector3f& position)
			{
				NzMeshVertex& vertex = m_vertices[m_vertexIndex];

				vertex.normal = NzVector3f::Normalize(m_matrix.Transform(position, 0.f));
				vertex.position = m_matrix.Transform(m_size * position.GetNormal());

				return m_vertexIndex++;
			}

			unsigned int GetMiddleVertex(unsigned int index1, unsigned int index2)
			{
				nzUInt64 key = (static_cast<nzUInt64>(std::min(index1, index2)) << 32) + static_cast<nzUInt32>(std::max(index1, index2));
				auto it = m_cache.find(key);
				if (it != m_cache.end())
					return it->second;

				NzVector3f middle = NzVector3f::Lerp(m_vertices[index1].position, m_vertices[index2].position, 0.5f);

				unsigned int index = AddVertex(middle);
				m_cache[key] = index;

				return index;
			}

		private:
			std::unordered_map<nzUInt64, unsigned int> m_cache;
			const NzMatrix4f& m_matrix;
			NzMeshVertex* m_vertices;
			float m_size;
			unsigned int m_vertexIndex;
	};
}

void NzComputeBoxIndexVertexCount(const NzVector3ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount)
{
	unsigned int xIndexCount, yIndexCount, zIndexCount;
	unsigned int xVertexCount, yVertexCount, zVertexCount;

	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision.y, subdivision.z), &xIndexCount, &xVertexCount);
	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision.x, subdivision.z), &yIndexCount, &yVertexCount);
	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision.x, subdivision.y), &zIndexCount, &zVertexCount);

	if (indexCount)
		*indexCount = xIndexCount*2 + yIndexCount*2 + zIndexCount*2;

	if (vertexCount)
		*vertexCount = xVertexCount*2 + yVertexCount*2 + zVertexCount*2;
}

void NzComputeCubicSphereIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount)
{
	// Comme tous nos plans sont identiques, on peut optimiser un peu
	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision), indexCount, vertexCount);

	if (indexCount)
		*indexCount *= 6;

	if (vertexCount)
		*vertexCount *= 6;
}

void NzComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, unsigned int* indexCount, unsigned int* vertexCount)
{
	if (indexCount)
		*indexCount = 3 * 20 * NzIntegralPow(4, recursionLevel);

	if (vertexCount)
		*vertexCount = NzIntegralPow(4, recursionLevel)*10 + 2;
}

void NzComputePlaneIndexVertexCount(const NzVector2ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount)
{
	// Le nombre de faces appartenant à un axe est équivalent à 2 exposant la subdivision (2,3,5,9,17,33,...)
	unsigned int horizontalFaceCount = (1 << subdivision.x);
	unsigned int verticalFaceCount = (1 << subdivision.y);

	// Et le nombre de sommets est ce nombre ajouté de 1
	unsigned int horizontalVertexCount = horizontalFaceCount + 1;
	unsigned int verticalVertexCount = verticalFaceCount + 1;

	if (indexCount)
		*indexCount = horizontalFaceCount*verticalFaceCount*6;

	if (vertexCount)
		*vertexCount = horizontalVertexCount*verticalVertexCount;
}

void NzComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, unsigned int* indexCount, unsigned int* vertexCount)
{
	if (indexCount)
		*indexCount = (sliceCount-1) * (stackCount-1) * 6;

	if (vertexCount)
		*vertexCount = sliceCount * stackCount;
}

void NzGenerateBox(const NzBoxf& box, const NzVector3ui& subdivision, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb, unsigned int indexOffset)
{
	unsigned int xIndexCount, yIndexCount, zIndexCount;
	unsigned int xVertexCount, yVertexCount, zVertexCount;

	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision.y, subdivision.z), &xIndexCount, &xVertexCount);
	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision.x, subdivision.z), &yIndexCount, &yVertexCount);
	NzComputePlaneIndexVertexCount(NzVector2ui(subdivision.x, subdivision.y), &zIndexCount, &zVertexCount);

	NzMeshVertex* oldVertices = vertices;

	// Face +X
	NzGeneratePlane(NzVector2ui(subdivision.y, subdivision.z), box.GetPosition() + NzVector3f::UnitX() * box.width/2.f, NzVector3f::UnitX(), NzVector2f(box.height, box.depth), vertices, indices, nullptr, indexOffset);
	indexOffset += xVertexCount;
	indices += xIndexCount;
	vertices += xVertexCount;

	// Face +Y
	NzGeneratePlane(NzVector2ui(subdivision.x, subdivision.z), box.GetPosition() + NzVector3f::UnitY() * box.height/2.f, NzVector3f::UnitY(), NzVector2f(box.width, box.depth), vertices, indices, nullptr, indexOffset);
	indexOffset += yVertexCount;
	indices += yIndexCount;
	vertices += yVertexCount;

	// Face +Z
	NzGeneratePlane(NzVector2ui(subdivision.x, subdivision.y), box.GetPosition() + NzVector3f::UnitZ() * box.depth/2.f, NzVector3f::UnitZ(), NzVector2f(box.width, box.height), vertices, indices, nullptr, indexOffset);
	indexOffset += zVertexCount;
	indices += zIndexCount;
	vertices += zVertexCount;

	// Face -X
	NzGeneratePlane(NzVector2ui(subdivision.y, subdivision.z), box.GetPosition() - NzVector3f::UnitX() * box.width/2.f, -NzVector3f::UnitX(), NzVector2f(box.height, box.depth), vertices, indices, nullptr, indexOffset);
	indexOffset += xVertexCount;
	indices += xIndexCount;
	vertices += xVertexCount;

	// Face -Y
	NzGeneratePlane(NzVector2ui(subdivision.x, subdivision.z), box.GetPosition() - NzVector3f::UnitY() * box.height/2.f, -NzVector3f::UnitY(), NzVector2f(box.width, box.depth), vertices, indices, nullptr, indexOffset);
	indexOffset += yVertexCount;
	indices += yIndexCount;
	vertices += yVertexCount;

	// Face -Z
	NzGeneratePlane(NzVector2ui(subdivision.x, subdivision.y), box.GetPosition() - NzVector3f::UnitZ() * box.depth/2.f, -NzVector3f::UnitZ(), NzVector2f(box.width, box.height), vertices, indices, nullptr, indexOffset);
	indexOffset += zVertexCount;
	indices += zIndexCount;
	vertices += zVertexCount;

	NzTransformVertices(oldVertices, vertices-oldVertices, matrix);

	if (aabb)
	{
		aabb->Set(NzVector3f::Unit());
		aabb->Transform(matrix, 0.f);
	}
}

void NzGenerateCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb, unsigned int indexOffset)
{
	unsigned int vertexCount;
	NzComputeBoxIndexVertexCount(NzVector3ui(subdivision), nullptr, &vertexCount);

	// On envoie une matrice identité de sorte à ce que le box ne subisse aucune transformation (rendant plus facile l'étape suivante)
	NzGenerateBox(NzBoxf(size, size, size), NzVector3ui(subdivision), NzMatrix4f::Identity(), vertices, indices, nullptr, indexOffset);

	if (aabb)
	{
		NzVector3f totalSize = size * matrix.GetScale();
		aabb->Set(-totalSize, totalSize);
	}

	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		vertices->normal = vertices->position.GetNormal();
		vertices->position = matrix.Transform(size * vertices->normal);
		//vertices->tangent = ???
		vertices++;
	}
}

void NzGenerateIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb, unsigned int indexOffset)
{
	IcoSphereBuilder builder(matrix);
	builder.Generate(size, recursionLevel, vertices, indices, aabb, indexOffset);
}

void NzGeneratePlane(const NzVector2ui& subdivision, const NzVector3f& position, const NzVector3f& normal, const NzVector2f& size, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb, unsigned int indexOffset)
{
	// Le nombre de faces appartenant à un axe est équivalent à 2 exposant la subdivision (2,3,5,9,17,33,...)
	unsigned int horizontalFaceCount = (1 << subdivision.x);
	unsigned int verticalFaceCount = (1 << subdivision.y);

	// Et le nombre de sommets est ce nombre ajouté de 1
	unsigned int horizontalVertexCount = horizontalFaceCount + 1;
	unsigned int verticalVertexCount = verticalFaceCount + 1;

	// Pour plus de facilité, on va construire notre plan en considérant que la normale est de 0,1,0
	// et on va construire un quaternion représentant la rotation de cette normale à la normale demandée par l'utilisateur.
	// Celui-ci, combiné avec la position, va former une transformation qu'il suffira d'appliquer aux sommets
	NzQuaternionf rotation;
	rotation.MakeRotationBetween(NzVector3f::UnitY(), normal);

	NzMatrix4f transform;
	transform.MakeTransform(position, rotation);

	///FIXME: Vérifier les tangentes
	NzVector3f tangent(1.f, 1.f, 0.f);
	tangent = rotation * tangent;
	tangent.Normalize();

	float halfSizeX = size.x / 2.f;
	float halfSizeY = size.y / 2.f;

	float invHorizontalVertexCount = 1.f/(horizontalVertexCount-1);
	float invVerticalVertexCount = 1.f/(verticalVertexCount-1);
	for (unsigned int x = 0; x < horizontalVertexCount; ++x)
	{
		for (unsigned int y = 0; y < verticalVertexCount; ++y)
		{
			NzVector3f localPos((2.f*x*invHorizontalVertexCount - 1.f) * halfSizeX, 0.f, (2.f*y*invVerticalVertexCount - 1.f) * halfSizeY);
			vertices->position = transform * localPos;
			vertices->uv.Set(x*invHorizontalVertexCount, y*invVerticalVertexCount);
			vertices->normal = normal;
			vertices->tangent = tangent;
			vertices++;

			if (x != horizontalVertexCount-1 && y != verticalVertexCount-1)
			{
				*indices++ = (x+0)*verticalVertexCount + y + 0 + indexOffset;
				*indices++ = (x+0)*verticalVertexCount + y + 1 + indexOffset;
				*indices++ = (x+1)*verticalVertexCount + y + 0 + indexOffset;

				*indices++ = (x+1)*verticalVertexCount + y + 0 + indexOffset;
				*indices++ = (x+0)*verticalVertexCount + y + 1 + indexOffset;
				*indices++ = (x+1)*verticalVertexCount + y + 1 + indexOffset;
			}
		}
	}

	if (aabb)
		aabb->Set(rotation * NzVector3f(-halfSizeX, 0.f, -halfSizeY), rotation * NzVector3f(halfSizeX, 0.f, halfSizeY));
}

void NzGenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& matrix, NzMeshVertex* vertices, nzUInt32* indices, NzBoxf* aabb, unsigned int indexOffset)
{
	// http://stackoverflow.com/questions/14080932/implementing-opengl-sphere-example-code
	float invSliceCount = 1.f / (sliceCount-1);
	float invStackCount = 1.f / (stackCount-1);

	const float pi = M_PI; // Pour éviter toute promotion en double
	const float pi2 = pi * 2.f;
	const float pi_2 = pi / 2.f;

	for (unsigned int stack = 0; stack < stackCount; ++stack)
	{
		float stackVal = stack * invStackCount;
		float stackValPi = stackVal * pi;
		float sinStackValPi = std::sin(stackValPi);

		for (unsigned int slice = 0; slice < sliceCount; ++slice)
		{
			float sliceVal = slice * invSliceCount;
			float sliceValPi2 = sliceVal * pi2;

			NzVector3f normal;
			normal.y = std::sin(-pi_2 + stackValPi);
			normal.x = std::cos(sliceValPi2) * sinStackValPi;
			normal.z = std::sin(sliceValPi2) * sinStackValPi;

			vertices->position = matrix.Transform(size * normal);
			vertices->normal = matrix.Transform(normal, 0.f);
			vertices->uv.Set(1.f - sliceVal, stackVal);
			vertices++;

			if (stack != stackCount-1 && slice != sliceCount-1)
			{
				*indices++ = (stack+0)*sliceCount + (slice+0) + indexOffset;
				*indices++ = (stack+1)*sliceCount + (slice+0) + indexOffset;
				*indices++ = (stack+0)*sliceCount + (slice+1) + indexOffset;

				*indices++ = (stack+0)*sliceCount + (slice+1) + indexOffset;
				*indices++ = (stack+1)*sliceCount + (slice+0) + indexOffset;
				*indices++ = (stack+1)*sliceCount + (slice+1) + indexOffset;
			}
		}
	}

	if (aabb)
	{
		NzVector3f totalSize = size * matrix.GetScale();
		aabb->Set(-totalSize, totalSize);
	}
}
