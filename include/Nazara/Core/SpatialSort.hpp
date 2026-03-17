// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SPATIALSORT_HPP
#define NAZARA_CORE_SPATIALSORT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/SparsePtr.hpp>

namespace Nz
{
	class NAZARA_CORE_API SpatialSort
	{
		public:
			inline SpatialSort(Vector3f planeNormal = Vector3f(0.99544406f, 0.099208f, 0.0021541119f));
			SpatialSort(const SpatialSort&) = delete;
			SpatialSort(SpatialSort&&) = delete;
			~SpatialSort() = default;

			void Append(SparsePtr<const Vector3f> vertices, UInt32 vertexCount, UInt32 indexOffset = 0, bool sort = true);

			void FindPositions(const Vector3f& position, float tolerance, std::vector<UInt32>& results) const;

			void Sort();

			SpatialSort& operator=(const SpatialSort&) = delete;
			SpatialSort& operator=(SpatialSort&&) = delete;

		private:
			struct Vertex
			{
				UInt32 index;
				Vector3f position;
				float distance;
			};

			std::vector<Vertex> m_vertices;
			Vector3f m_planeNormal;
			bool m_isSorted;
	};
}

#include <Nazara/Core/SpatialSort.inl>

#endif // NAZARA_CORE_SPATIALSORT_HPP
