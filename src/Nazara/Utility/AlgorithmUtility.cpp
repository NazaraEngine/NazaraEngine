// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

/*
 * vcacheopt.h - Vertex Cache Optimizer
 * Copyright 2009 Michael Georgoulpoulos <mgeorgoulopoulos at gmail>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Nazara/Utility/Algorithm.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <algorithm>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>

namespace Nz
{
	namespace
	{
		class IcoSphereBuilder
		{
			public:
				explicit IcoSphereBuilder(const Matrix4f& matrix) :
				m_matrix(matrix)
				{
				}

				void Generate(float size, unsigned int recursionLevel, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
				{
					// Grandement inspiré de http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
					const float t = (1.f + 2.236067f)/2.f;

					m_cache.clear();
					m_size = size;
					m_vertices = vertexPointers;
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

					std::vector<Vector3ui> triangles;
					triangles.reserve(20 * IntegralPow(4, recursionLevel));

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
						std::size_t triangleCount = triangles.size();
						for (std::size_t j = 0; j < triangleCount; ++j)
						{
							Vector3ui& triangle = triangles[j];

							unsigned int a = GetMiddleVertex(triangle.x, triangle.y);
							unsigned int b = GetMiddleVertex(triangle.y, triangle.z);
							unsigned int c = GetMiddleVertex(triangle.z, triangle.x);

							triangles.push_back({triangle.x, a, c});
							triangles.push_back({triangle.y, b, a});
							triangles.push_back({triangle.z, c, b});

							triangle.Set(a, b, c); // Réutilisation du triangle
						}
					}

					for (const Vector3ui& triangle : triangles)
					{
						*indices++ = triangle.x + indexOffset;
						*indices++ = triangle.y + indexOffset;
						*indices++ = triangle.z + indexOffset;
					}

					if (aabb)
					{
						Vector3f totalSize = size * m_matrix.GetScale();
						aabb->Set(-totalSize, totalSize);
					}
				}

				unsigned int AddVertex(const Vector3f& position)
				{
					m_vertices.normalPtr[m_vertexIndex] = Vector3f::Normalize(m_matrix.Transform(position, 0.f));
					m_vertices.positionPtr[m_vertexIndex] = m_matrix.Transform(m_size * position.GetNormal());

					return m_vertexIndex++;
				}

				unsigned int GetMiddleVertex(unsigned int index1, unsigned int index2)
				{
					UInt64 key = (static_cast<UInt64>(std::min(index1, index2)) << 32) + static_cast<UInt32>(std::max(index1, index2));
					auto it = m_cache.find(key);
					if (it != m_cache.end())
						return it->second;

					Vector3f middle = Vector3f::Lerp(m_vertices.positionPtr[index1], m_vertices.positionPtr[index2], 0.5f);

					unsigned int index = AddVertex(middle);
					m_cache[key] = index;

					return index;
				}

			private:
				std::unordered_map<UInt64, unsigned int> m_cache;
				const Matrix4f& m_matrix;
				VertexPointers m_vertices;
				float m_size;
				unsigned int m_vertexIndex;
		};

		// Source: https://code.google.com/p/vcacne/
		// Auteur: Michael Georgoulpoulos
		// Selon ce papier: http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
		// Modifié pour les besoins du moteur
		///TODO: Déplacer dans un fichier à part ?
		struct VertexCacheData
		{
			int position_in_cache = -1;
			float current_score = 0.f;
			int total_valence = 0; // toatl number of triangles using this vertex
			int remaining_valence = 0; // number of triangles using it but not yet rendered
			std::vector<int> tri_indices; // indices to the indices that use this vertex
			bool calculated; // was the score calculated during this iteration?


			int FindTriangle(int tri)
			{
				for (unsigned int i = 0; i < tri_indices.size(); ++i)
					if (tri_indices[i] == tri) return i;

				return -1;
			}

			void MoveTriangleToEnd(int tri)
			{
				auto it = std::find(tri_indices.begin(), tri_indices.end(), tri);
				NazaraAssert(it != tri_indices.end(), "Triangle not found");

				tri_indices.erase(it);
				tri_indices.push_back(tri);
			}
		};

		struct TriangleCacheData
		{
			bool rendered = false; // has the triangle been added to the draw list yet?
			float current_score = 0.f; // sum of the score of its vertices
			int verts[3] = {-1, -1, -1}; // indices to the triangle's vertices
			bool calculated = false; // was the score calculated during this iteration?
		};

		class VertexCache
		{
			public:
				VertexCache()
				{
					Clear();
				}

				VertexCache(IndexIterator indices, unsigned int indexCount)
				{
					Clear();

					for (unsigned int i = 0; i < indexCount; ++i)
						AddVertex(*indices++);
				}

				// the vertex will be placed on top
				// if the vertex didn't exist previewsly in
				// the cache, then miss count is incermented
				void AddVertex(unsigned int v)
				{
					int w = FindVertex(v);
					if (w >= 0)
						// remove the vertex from the cache (to reinsert it later on the top)
						RemoveVertex(w);
					else
						// the vertex was not found in the cache - increment misses
						m_misses++;

					// shift all vertices down (to make room for the new top vertex)
					for (int i=39; i>0; i--)
						m_cache[i] = m_cache[i-1];

					// add the new vertex on top
					m_cache[0] = v;
				}

				void Clear()
				{
					for (int i=0; i<40; i++)
						m_cache[i] = -1;

					m_misses = 0;
				}

				int GetMissCount() const
				{
					return m_misses;
				}

				int GetVertex(int which) const
				{
					return m_cache[which];
				}

			private:
				int FindVertex(int v) const
				{
					for (int i = 0; i < 32; ++i)
					{
						if (m_cache[i] == v)
							return i;
					}

					return -1;
				}

				void RemoveVertex(int stack_index)
				{
					for (int i=stack_index; i<38; i++)
						m_cache[i] = m_cache[i+1];
				}

				int m_cache[40];
				int m_misses; // cache miss count
		};

		class VertexCacheOptimizer
		{
			public:
				enum Result
				{
					Success,
					Fail_BadIndex,
					Fail_NoVerts
				};

				VertexCacheOptimizer()
				{
					// initialize constants
					m_cacheDecayPower = 1.5f;
					m_lastTriScore = 0.75f;
					m_valenceBoostScale = 2.0f;
					m_valenceBoostPower = 0.5f;

					m_bestTri = 0;
				}

				// stores new indices in place
				Result Optimize(IndexIterator indices, unsigned int indexCount)
				{
					if (indexCount == 0)
						return Fail_NoVerts;

					// find vertex count
					int max_vert = *std::max_element(indices, indices + indexCount);

					Result res = Init(indices, indexCount, max_vert + 1);
					if (res != Success)
						return res;

					// iterate until Iterate returns false
					while (Iterate());

					// rewrite optimized index list
					for (int index : m_drawList)
					{
						*indices++ = m_triangles[index].verts[0];
						*indices++ = m_triangles[index].verts[1];
						*indices++ = m_triangles[index].verts[2];
					}

					return Success;
				}

			private:
				float CalculateVertexScore(VertexCacheData& vertex) const
				{
					if (vertex.remaining_valence <= 0)
						// No tri needs this vertex!
						return -1.0f;

					float ret = 0.0f;
					if (vertex.position_in_cache < 0)
					{
						// Vertex is not in FIFO cache - no score.
					}
					else
					{
						if (vertex.position_in_cache < 3)
						{
							// This vertex was used in the last triangle,
							// so it has a fixed score, whichever of the three
							// it's in. Otherwise, you can get very different
							// answers depending on whether you add
							// the triangle 1,2,3 or 3,1,2 - which is silly.
							ret = m_lastTriScore;
						}
						else
						{
							// Points for being high in the cache.
							const float Scaler = 1.0f / (32  - 3);
							ret = 1.0f - (vertex.position_in_cache - 3) * Scaler;
							ret = std::pow(ret, m_cacheDecayPower);
						}
					}

					// Bonus points for having a low number of tris still to
					// use the vert, so we get rid of lone verts quickly.
					float valence_boost = std::pow(static_cast<float>(vertex.remaining_valence), -m_valenceBoostPower);
					ret += m_valenceBoostScale * valence_boost;

					return ret;
				}

				// returns the index of the triangle with the highest score
				// (or -1, if there aren't any active triangles)
				int FullScoreRecalculation()
				{
					// calculate score for all vertices
					for (VertexCacheData& vertex : m_vertices)
						vertex.current_score = CalculateVertexScore(vertex);

					// calculate scores for all active triangles
					float max_score = std::numeric_limits<float>::lowest();
					int max_score_tri = -1;

					for (unsigned int i = 0; i < m_triangles.size(); ++i)
					{
						if (m_triangles[i].rendered)
							continue;

						// sum the score of all the triangle's vertices
						float sc = m_vertices[m_triangles[i].verts[0]].current_score +
								   m_vertices[m_triangles[i].verts[1]].current_score +
								   m_vertices[m_triangles[i].verts[2]].current_score;

						m_triangles[i].current_score = sc;

						if (sc > max_score)
						{
							max_score = sc;
							max_score_tri = i;
						}
					}

					return max_score_tri;
				}

				Result InitialPass()
				{
					for (unsigned int i = 0; i < m_indices.size(); ++i)
					{
						int index = m_indices[i];
						if (index < 0 || index >= static_cast<int>(m_vertices.size()))
							return Fail_BadIndex;

						m_vertices[index].total_valence++;
						m_vertices[index].remaining_valence++;

						m_vertices[index].tri_indices.push_back(i/3);
					}

					m_bestTri = FullScoreRecalculation();

					return Success;
				}

				Result Init(IndexIterator indices, unsigned int indexCount, int vertex_count)
				{
					// clear the draw list
					m_drawList.clear();

					// allocate and initialize vertices and triangles
					m_vertices.clear(); // Pour reconstruire tous les éléments
					m_vertices.resize(vertex_count);

					m_triangles.clear();
					for (unsigned int i = 0; i < indexCount; i += 3)
					{
						TriangleCacheData dat;
						for (unsigned int j = 0; j < 3; ++j)
							dat.verts[j] = indices[i + j];

						m_triangles.push_back(dat);
					}

					// copy the indices
					m_indices.resize(indexCount);
					for (unsigned int i = 0; i < indexCount; ++i)
						m_indices[i] = indices[i];

					m_vertexCache.Clear();
					m_bestTri = -1;

					return InitialPass();
				}

				void AddTriangleToDrawList(int tri)
				{
					// reset all cache positions
					for (unsigned int i = 0; i < 32; ++i)
					{
						int ind = m_vertexCache.GetVertex(i);
						if (ind < 0)
							continue;

						m_vertices[ind].position_in_cache = -1;
					}

					TriangleCacheData* t = &m_triangles[tri];
					if (t->rendered)
						return; // triangle is already in the draw list

					for (unsigned int i = 0; i < 3; ++i)
					{
						// add all triangle vertices to the cache
						m_vertexCache.AddVertex(t->verts[i]);

						VertexCacheData *v = &m_vertices[t->verts[i]];

						// decrease remaining velence
						v->remaining_valence--;

						// move the added triangle to the end of the vertex's
						// triangle index list, so that the first 'remaining_valence'
						// triangles in the list are the active ones
						v->MoveTriangleToEnd(tri);
					}

					m_drawList.push_back(tri);

					t->rendered = true;

					// update all vertex cache positions
					for (unsigned int i = 0; i < 32; ++i)
					{
						int ind = m_vertexCache.GetVertex(i);
						if (ind < 0)
							continue;

						m_vertices[ind].position_in_cache = i;
					}
				}

				// Optimization: to avoid duplicate calculations durind the same iteration,
				// both vertices and triangles have a 'calculated' flag. This flag
				// must be cleared at the beginning of the iteration to all *active* triangles
				// that have one or more of their vertices currently cached, and all their
				// other vertices.
				// If there aren't any active triangles in the cache, the function returns
				// false and full recalculation is performed.
				bool CleanCalculationFlags()
				{
					bool ret = false;
					for (unsigned int i = 0; i < 32; ++i)
					{
						int vert = m_vertexCache.GetVertex(i);
						if (vert < 0)
							continue;

						VertexCacheData *v = &m_vertices[vert];

						for (int j = 0; j < v->remaining_valence; j++)
						{
							TriangleCacheData *t = &m_triangles[v->tri_indices[j]];

							// we actually found a triangle to process
							ret = true;

							// clear triangle flag
							t->calculated = false;

							// clear vertex flags
							for (unsigned int k = 0; k < 3; ++k)
								m_vertices[t->verts[k]].calculated = false;
						}
					}

					return ret;
				}

				void TriangleScoreRecalculation(int tri)
				{
					TriangleCacheData* t = &m_triangles[tri];

					// calculate vertex scores
					float sum = 0.f;
					for (unsigned int i = 0; i < 3; ++i)
					{
						VertexCacheData& v = m_vertices[t->verts[i]];
						float sc = v.current_score;
						if (!v.calculated)
							sc = CalculateVertexScore(v);

						v.current_score = sc;
						v.calculated = true;
						sum += sc;
					}

					t->current_score = sum;
					t->calculated = true;
				}

				int PartialScoreRecalculation()
				{
					// iterate through all the vertices of the cache
					float max_score = std::numeric_limits<float>::lowest();
					int max_score_tri = -1;

					for (unsigned int i = 0; i < 32; ++i)
					{
						int vert = m_vertexCache.GetVertex(i);
						if (vert < 0)
							continue;

						const VertexCacheData* v = &m_vertices[vert];

						// iterate through all *active* triangles of this vertex
						for (int j = 0; j < v->remaining_valence; j++)
						{
							int tri = v->tri_indices[j];
							TriangleCacheData* t = &m_triangles[tri];
							if (!t->calculated)
								// calculate triangle score
								TriangleScoreRecalculation(tri);

							float sc = t->current_score;

							// we actually found a triangle to process
							if (sc > max_score)
							{
								max_score = sc;
								max_score_tri = tri;
							}
						}
					}

					return max_score_tri;
				}

				// returns true while there are more steps to take
				// false when optimization is complete
				bool Iterate()
				{
					if (m_drawList.size() == m_triangles.size())
						return false;

					// add the selected triangle to the draw list
					AddTriangleToDrawList(m_bestTri);

					// recalculate vertex and triangle scores and
					// select the best triangle for the next iteration
					m_bestTri = (CleanCalculationFlags()) ? PartialScoreRecalculation() : FullScoreRecalculation();

					return true;
				}

				std::vector<VertexCacheData> m_vertices;
				std::vector<TriangleCacheData> m_triangles;
				std::vector<int> m_indices;
				int m_bestTri; // the next triangle to add to the render list
				VertexCache m_vertexCache;
				std::vector<int> m_drawList;

				// CalculateVertexScore constants
				float m_cacheDecayPower;
				float m_lastTriScore;
				float m_valenceBoostScale;
				float m_valenceBoostPower;
		};
	}

	/**********************************Compute**********************************/

	Boxf ComputeAABB(SparsePtr<const Vector3f> positionPtr, unsigned int vertexCount)
	{
		Boxf aabb;
		if (vertexCount > 0)
		{
			aabb.Set(positionPtr->x, positionPtr->y, positionPtr->z, 0.f, 0.f, 0.f);
			++positionPtr;

			for (unsigned int i = 1; i < vertexCount; ++i)
				aabb.ExtendTo(*positionPtr++);
		}
		else
			aabb.MakeZero();

		return aabb;
	}

	void ComputeBoxIndexVertexCount(const Vector3ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount)
	{
		unsigned int xIndexCount, yIndexCount, zIndexCount;
		unsigned int xVertexCount, yVertexCount, zVertexCount;

		ComputePlaneIndexVertexCount(Vector2ui(subdivision.y, subdivision.z), &xIndexCount, &xVertexCount);
		ComputePlaneIndexVertexCount(Vector2ui(subdivision.x, subdivision.z), &yIndexCount, &yVertexCount);
		ComputePlaneIndexVertexCount(Vector2ui(subdivision.x, subdivision.y), &zIndexCount, &zVertexCount);

		if (indexCount)
			*indexCount = xIndexCount*2 + yIndexCount*2 + zIndexCount*2;

		if (vertexCount)
			*vertexCount = xVertexCount*2 + yVertexCount*2 + zVertexCount*2;
	}

	unsigned int ComputeCacheMissCount(IndexIterator indices, unsigned int indexCount)
	{
		VertexCache cache(indices, indexCount);
		return cache.GetMissCount();
	}

	void ComputeConeIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount)
	{
		if (indexCount)
			*indexCount = (subdivision-1)*6;

		if (vertexCount)
			*vertexCount = subdivision + 2;
	}

	void ComputeCubicSphereIndexVertexCount(unsigned int subdivision, unsigned int* indexCount, unsigned int* vertexCount)
	{
		// Comme tous nos plans sont identiques, on peut optimiser un peu
		ComputePlaneIndexVertexCount(Vector2ui(subdivision), indexCount, vertexCount);

		if (indexCount)
			*indexCount *= 6;

		if (vertexCount)
			*vertexCount *= 6;
	}

	void ComputeIcoSphereIndexVertexCount(unsigned int recursionLevel, unsigned int* indexCount, unsigned int* vertexCount)
	{
		if (indexCount)
			*indexCount = 3 * 20 * IntegralPow(4, recursionLevel);

		if (vertexCount)
			*vertexCount = IntegralPow(4, recursionLevel)*10 + 2;
	}

	void ComputePlaneIndexVertexCount(const Vector2ui& subdivision, unsigned int* indexCount, unsigned int* vertexCount)
	{
		// Le nombre de faces appartenant à un axe est équivalent à 2 exposant la subdivision (1,2,4,8,16,32,...)
		unsigned int horizontalFaceCount = (1 << subdivision.x);
		unsigned int verticalFaceCount = (1 << subdivision.y);

		// Et le nombre de sommets est ce nombre ajouté de 1 (2,3,5,9,17,33,...)
		unsigned int horizontalVertexCount = horizontalFaceCount + 1;
		unsigned int verticalVertexCount = verticalFaceCount + 1;

		if (indexCount)
			*indexCount = horizontalFaceCount*verticalFaceCount*6; // Six indices sont nécessaires pour décrire une face (deux triangles)

		if (vertexCount)
			*vertexCount = horizontalVertexCount*verticalVertexCount;
	}

	void ComputeUvSphereIndexVertexCount(unsigned int sliceCount, unsigned int stackCount, unsigned int* indexCount, unsigned int* vertexCount)
	{
		if (indexCount)
			*indexCount = (sliceCount-1) * (stackCount-1) * 6;

		if (vertexCount)
			*vertexCount = sliceCount * stackCount;
	}

	/**********************************Generate*********************************/

	void GenerateBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
	{
		unsigned int xIndexCount, yIndexCount, zIndexCount;
		unsigned int xVertexCount, yVertexCount, zVertexCount;

		ComputePlaneIndexVertexCount(Vector2ui(subdivision.y, subdivision.z), &xIndexCount, &xVertexCount);
		ComputePlaneIndexVertexCount(Vector2ui(subdivision.x, subdivision.z), &yIndexCount, &yVertexCount);
		ComputePlaneIndexVertexCount(Vector2ui(subdivision.x, subdivision.y), &zIndexCount, &zVertexCount);

		Matrix4f transform;
		Vector3f halfLengths = lengths/2.f;

		// Face +X
		transform.MakeTransform(Vector3f::UnitX() * halfLengths.x, EulerAnglesf(-90.f, -90.f, 180.f));
		GeneratePlane(Vector2ui(subdivision.z, subdivision.y), Vector2f(lengths.z, lengths.y), Matrix4f::ConcatenateAffine(matrix, transform), textureCoords, vertexPointers, indices, nullptr, indexOffset);
		indexOffset += xVertexCount;
		indices += xIndexCount;

		vertexPointers.positionPtr += xVertexCount;

		if (vertexPointers.normalPtr)
			vertexPointers.normalPtr += xVertexCount;

		if (vertexPointers.tangentPtr)
			vertexPointers.tangentPtr += xVertexCount;

		if (vertexPointers.uvPtr)
			vertexPointers.uvPtr += xVertexCount;

		// Face +Y
		transform.MakeTransform(Vector3f::UnitY() * halfLengths.y, EulerAnglesf(0.f, 0.f, 0.f));
		GeneratePlane(Vector2ui(subdivision.x, subdivision.z), Vector2f(lengths.x, lengths.z), Matrix4f::ConcatenateAffine(matrix, transform), textureCoords, vertexPointers, indices, nullptr, indexOffset);
		indexOffset += yVertexCount;
		indices += yIndexCount;

		vertexPointers.positionPtr += yVertexCount;

		if (vertexPointers.normalPtr)
			vertexPointers.normalPtr += yVertexCount;

		if (vertexPointers.tangentPtr)
			vertexPointers.tangentPtr += yVertexCount;

		if (vertexPointers.uvPtr)
			vertexPointers.uvPtr += yVertexCount;

		// Face +Z
		transform.MakeTransform(Vector3f::UnitZ() * halfLengths.z, EulerAnglesf(90.f, 0.f, 0.f));
		GeneratePlane(Vector2ui(subdivision.x, subdivision.y), Vector2f(lengths.x, lengths.y), Matrix4f::ConcatenateAffine(matrix, transform), textureCoords, vertexPointers, indices, nullptr, indexOffset);
		indexOffset += zVertexCount;
		indices += zIndexCount;

		vertexPointers.positionPtr += zVertexCount;

		if (vertexPointers.normalPtr)
			vertexPointers.normalPtr += zVertexCount;

		if (vertexPointers.tangentPtr)
			vertexPointers.tangentPtr += zVertexCount;

		if (vertexPointers.uvPtr)
			vertexPointers.uvPtr += zVertexCount;

		// Face -X
		transform.MakeTransform(-Vector3f::UnitX() * halfLengths.x, EulerAnglesf(-90.f, 90.f, 180.f));
		GeneratePlane(Vector2ui(subdivision.z, subdivision.y), Vector2f(lengths.z, lengths.y), Matrix4f::ConcatenateAffine(matrix, transform), textureCoords, vertexPointers, indices, nullptr, indexOffset);
		indexOffset += xVertexCount;
		indices += xIndexCount;

		vertexPointers.positionPtr += xVertexCount;

		if (vertexPointers.normalPtr)
			vertexPointers.normalPtr += xVertexCount;

		if (vertexPointers.tangentPtr)
			vertexPointers.tangentPtr += xVertexCount;

		if (vertexPointers.uvPtr)
			vertexPointers.uvPtr += xVertexCount;

		// Face -Y
		transform.MakeTransform(-Vector3f::UnitY() * halfLengths.y, EulerAnglesf(0.f, 180.f, 180.f));
		GeneratePlane(Vector2ui(subdivision.x, subdivision.z), Vector2f(lengths.x, lengths.z), Matrix4f::ConcatenateAffine(matrix, transform), textureCoords, vertexPointers, indices, nullptr, indexOffset);
		indexOffset += yVertexCount;
		indices += yIndexCount;

		vertexPointers.positionPtr += yVertexCount;

		if (vertexPointers.normalPtr)
			vertexPointers.normalPtr += yVertexCount;

		if (vertexPointers.tangentPtr)
			vertexPointers.tangentPtr += yVertexCount;

		if (vertexPointers.uvPtr)
			vertexPointers.uvPtr += yVertexCount;

		// Face -Z
		transform.MakeTransform(-Vector3f::UnitZ() * halfLengths.z, EulerAnglesf(90.f, 180.f, 0.f));
		GeneratePlane(Vector2ui(subdivision.x, subdivision.y), Vector2f(lengths.x, lengths.y), Matrix4f::ConcatenateAffine(matrix, transform), textureCoords, vertexPointers, indices, nullptr, indexOffset);
		indexOffset += zVertexCount;
		indices += zIndexCount;

		vertexPointers.positionPtr += zVertexCount;

		if (vertexPointers.normalPtr)
			vertexPointers.normalPtr += zVertexCount;

		if (vertexPointers.tangentPtr)
			vertexPointers.tangentPtr += zVertexCount;

		if (vertexPointers.uvPtr)
			vertexPointers.uvPtr += zVertexCount;

		if (aabb)
		{
			aabb->Set(-halfLengths, halfLengths);
			aabb->Transform(matrix, false);
		}
	}

	void GenerateCone(float length, float radius, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
	{
		constexpr float round = 2.f*static_cast<float>(M_PI);
		float delta = round/subdivision;

		*vertexPointers.positionPtr++ = matrix.GetTranslation(); // matrix.Transform(Vector3f(0.f));

		if (vertexPointers.normalPtr)
			*vertexPointers.normalPtr++ = matrix.Transform(Vector3f::Up(), 0.f);

		for (unsigned int i = 0; i < subdivision; ++i)
		{
			float angle = delta*i;
			*vertexPointers.positionPtr++ = matrix.Transform(Vector3f(radius*std::sin(angle), -length, radius*std::cos(angle)));

			*indices++ = indexOffset + 0;
			*indices++ = indexOffset + i+1;
			*indices++ = indexOffset + ((i != subdivision-1) ? i+2 : 1);

			if (i != 0 && i != subdivision-1)
			{
				*indices++ = indexOffset + ((i != subdivision-1) ? i+2 : 1);
				*indices++ = indexOffset + i+1;
				*indices++ = indexOffset + 1;
			}
		}

		if (aabb)
		{
			aabb->MakeZero();

			// On calcule le reste des points
			Vector3f base(Vector3f::Down()*length);

			Vector3f lExtend = Vector3f::Left()*radius;
			Vector3f fExtend = Vector3f::Forward()*radius;

			// Et on ajoute ensuite les quatres extrémités de la pyramide
			aabb->ExtendTo(base + lExtend + fExtend);
			aabb->ExtendTo(base + lExtend - fExtend);
			aabb->ExtendTo(base - lExtend + fExtend);
			aabb->ExtendTo(base - lExtend - fExtend);
			aabb->Transform(matrix, false);
		}
	}

	void GenerateCubicSphere(float size, unsigned int subdivision, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
	{
		///DOC: Cette fonction va accéder aux pointeurs en écriture ET en lecture
		unsigned int vertexCount;
		ComputeBoxIndexVertexCount(Vector3ui(subdivision), nullptr, &vertexCount);

		// On envoie une matrice identité de sorte à ce que la boîte ne subisse aucune transformation (rendant plus facile l'étape suivante)
		GenerateBox(Vector3f(size, size, size), Vector3ui(subdivision), Matrix4f::Identity(), textureCoords, vertexPointers, indices, nullptr, indexOffset);

		if (aabb)
		{
			Vector3f totalSize = size * matrix.GetScale();
			aabb->Set(-totalSize, totalSize);
		}

		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			Vector3f normal = vertexPointers.positionPtr->GetNormal();

			*vertexPointers.positionPtr++ = matrix.Transform(size * normal);

			if (vertexPointers.normalPtr)
				*vertexPointers.normalPtr++ = normal;

			///FIXME: *vertexPointers.tangentPtr++ = ???
		}
	}

	void GenerateIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
	{
		IcoSphereBuilder builder(matrix);
		builder.Generate(size, recursionLevel, textureCoords, vertexPointers, indices, aabb, indexOffset);
	}

	void GeneratePlane(const Vector2ui& subdivision, const Vector2f& size, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
	{
		// Pour plus de facilité, on va construire notre plan en considérant que la normale est de 0,1,0
		// Et appliquer ensuite une matrice "finissant le travail"

		// Le nombre de faces appartenant à un axe est équivalent à 2 exposant la subdivision (1,2,4,8,16,32,...)
		unsigned int horizontalFaceCount = (1 << subdivision.x);
		unsigned int verticalFaceCount = (1 << subdivision.y);

		// Et le nombre de sommets est ce nombre ajouté de 1 (2,3,5,9,17,33,...)
		unsigned int horizontalVertexCount = horizontalFaceCount + 1;
		unsigned int verticalVertexCount = verticalFaceCount + 1;

		Vector3f normal(Vector3f::UnitY());
		normal = matrix.Transform(normal, 0.f);
		normal.Normalize();

		Vector3f tangent(1.f, 1.f, 0.f);
		tangent = matrix.Transform(tangent, 0.f);
		tangent.Normalize();

		float halfSizeX = size.x / 2.f;
		float halfSizeY = size.y / 2.f;

		float invHorizontalVertexCount = 1.f/(horizontalVertexCount-1);
		float invVerticalVertexCount = 1.f/(verticalVertexCount-1);
		for (unsigned int x = 0; x < horizontalVertexCount; ++x)
		{
			for (unsigned int y = 0; y < verticalVertexCount; ++y)
			{
				Vector3f localPos((2.f*x*invHorizontalVertexCount - 1.f) * halfSizeX, 0.f, (2.f*y*invVerticalVertexCount - 1.f) * halfSizeY);
				*vertexPointers.positionPtr++ = matrix * localPos;

				if (vertexPointers.normalPtr)
					*vertexPointers.normalPtr++ = normal;

				if (vertexPointers.tangentPtr)
					*vertexPointers.tangentPtr++ = tangent;

				if (vertexPointers.uvPtr)
					*vertexPointers.uvPtr++ = Vector2f(textureCoords.x + x*invHorizontalVertexCount*textureCoords.width, textureCoords.y + y*invVerticalVertexCount*textureCoords.height);

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
			aabb->Set(matrix.Transform(Vector3f(-halfSizeX, 0.f, -halfSizeY), 0.f), matrix.Transform(Vector3f(halfSizeX, 0.f, halfSizeY), 0.f));
	}

	void GenerateUvSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& matrix, const Rectf& textureCoords, VertexPointers vertexPointers, IndexIterator indices, Boxf* aabb, unsigned int indexOffset)
	{
		// http://stackoverflow.com/questions/14080932/implementing-opengl-sphere-example-code
		float invSliceCount = 1.f / (sliceCount-1);
		float invStackCount = 1.f / (stackCount-1);

		const float pi = static_cast<float>(M_PI); // Pour éviter toute promotion en double
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

				Vector3f normal;
				normal.y = std::sin(-pi_2 + stackValPi);
				normal.x = std::cos(sliceValPi2) * sinStackValPi;
				normal.z = std::sin(sliceValPi2) * sinStackValPi;

				*vertexPointers.positionPtr++ = matrix.Transform(size * normal);

				if (vertexPointers.normalPtr)
					*vertexPointers.normalPtr++ = matrix.Transform(normal, 0.f);

				if (vertexPointers.uvPtr)
					*vertexPointers.uvPtr++ = Vector2f(textureCoords.x + textureCoords.width*(1.f - sliceVal), textureCoords.y + textureCoords.height*stackVal);

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
			Vector3f totalSize = size * matrix.GetScale();
			aabb->Set(-totalSize, totalSize);
		}
	}

	/**********************************Optimize*********************************/

	void OptimizeIndices(IndexIterator indices, unsigned int indexCount)
	{
		VertexCacheOptimizer optimizer;
		if (optimizer.Optimize(indices, indexCount) != VertexCacheOptimizer::Success)
			NazaraWarning("Indices optimizer failed");
	}

	/************************************Skin***********************************/

	void SkinPosition(const SkinningData& skinningInfos, unsigned int startVertex, unsigned int vertexCount)
	{
		const SkeletalMeshVertex* inputVertex = &skinningInfos.inputVertex[startVertex];
		MeshVertex* outputVertex = &skinningInfos.outputVertex[startVertex];

		unsigned int endVertex = startVertex + vertexCount - 1;
		for (unsigned int i = startVertex; i <= endVertex; ++i)
		{
			Vector3f finalPosition(Vector3f::Zero());

			for (int j = 0; j < inputVertex->weightCount; ++j)
			{
				Matrix4f mat(skinningInfos.joints[inputVertex->jointIndexes[j]].GetSkinningMatrix());
				mat *= inputVertex->weights[j];

				finalPosition += mat.Transform(inputVertex->position);
			}

			outputVertex->position = finalPosition;
			outputVertex->uv = inputVertex->uv;

			inputVertex++;
			outputVertex++;
		}
	}

	void SkinPositionNormal(const SkinningData& skinningInfos, unsigned int startVertex, unsigned int vertexCount)
	{
		const SkeletalMeshVertex* inputVertex = &skinningInfos.inputVertex[startVertex];
		MeshVertex* outputVertex = &skinningInfos.outputVertex[startVertex];

		unsigned int endVertex = startVertex + vertexCount - 1;
		for (unsigned int i = startVertex; i <= endVertex; ++i)
		{
			Vector3f finalPosition(Vector3f::Zero());
			Vector3f finalNormal(Vector3f::Zero());

			for (int j = 0; j < inputVertex->weightCount; ++j)
			{
				Matrix4f mat(skinningInfos.joints[inputVertex->jointIndexes[j]].GetSkinningMatrix());
				mat *= inputVertex->weights[j];

				finalPosition += mat.Transform(inputVertex->position);
				finalNormal += mat.Transform(inputVertex->normal, 0.f);
			}

			finalNormal.Normalize();

			outputVertex->normal = finalNormal;
			outputVertex->position = finalPosition;
			outputVertex->uv = inputVertex->uv;

			inputVertex++;
			outputVertex++;
		}
	}

	void SkinPositionNormalTangent(const SkinningData& skinningInfos, unsigned int startVertex, unsigned int vertexCount)
	{
		const SkeletalMeshVertex* inputVertex = &skinningInfos.inputVertex[startVertex];
		MeshVertex* outputVertex = &skinningInfos.outputVertex[startVertex];

		unsigned int endVertex = startVertex + vertexCount - 1;
		for (unsigned int i = startVertex; i <= endVertex; ++i)
		{
			Vector3f finalPosition(Vector3f::Zero());
			Vector3f finalNormal(Vector3f::Zero());
			Vector3f finalTangent(Vector3f::Zero());

			for (int j = 0; j < inputVertex->weightCount; ++j)
			{
				Matrix4f mat(skinningInfos.joints[inputVertex->jointIndexes[j]].GetSkinningMatrix());
				mat *= inputVertex->weights[j];

				finalPosition += mat.Transform(inputVertex->position);
				finalNormal += mat.Transform(inputVertex->normal, 0.f);
				finalTangent += mat.Transform(inputVertex->tangent, 0.f);
			}

			finalNormal.Normalize();
			finalTangent.Normalize();

			outputVertex->normal = finalNormal;
			outputVertex->position = finalPosition;
			outputVertex->tangent = finalTangent;
			outputVertex->uv = inputVertex->uv;

			inputVertex++;
			outputVertex++;
		}
	}

	/*********************************Transform*********************************/

	void TransformVertices(VertexPointers vertexPointers, unsigned int vertexCount, const Matrix4f& matrix)
	{
		///DOC: Pointeur read/write
		Vector3f scale = matrix.GetScale();

		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			*vertexPointers.positionPtr++ = matrix.Transform(*vertexPointers.positionPtr);

			if (vertexPointers.normalPtr)
				*vertexPointers.normalPtr++ = matrix.Transform(*vertexPointers.normalPtr, 0.f) / scale;

			if (vertexPointers.tangentPtr)
				*vertexPointers.tangentPtr++ = matrix.Transform(*vertexPointers.tangentPtr, 0.f) / scale;
		}
	}
}
